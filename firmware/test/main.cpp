#include <Arduino.h>
#include <Wire.h>
#include <SerialFlash.h>

#include "debug.h"
#include "board_definition.h"
#include "two_wire.h"

constexpr uint8_t AtlasHigh = 1;
constexpr uint8_t AtlasLow = 0;

fk::TwoWireBus bus{ Wire };

#define FK_ATLAS_OEM

const uint8_t ATLAS_RESPONSE_CODE_NO_DATA = 0xff;
const uint8_t ATLAS_RESPONSE_CODE_NOT_READY = 0xfe;
const uint8_t ATLAS_RESPONSE_CODE_ERROR = 0x2;
const uint8_t ATLAS_RESPONSE_CODE_SUCCESS = 0x1;

class AtlasScientificBoard {
    virtual bool begin() = 0;
    virtual bool reading() = 0;
    virtual bool find() = 0;
    virtual bool status() = 0;
    virtual bool info() = 0;
    virtual bool ledsOn() = 0;
    virtual bool ledsOff() = 0;
    virtual bool configure() = 0;
};

class AtlasScientificOemBoard : public AtlasScientificBoard {
private:
    uint8_t address_;
    uint8_t type_;

private:
    struct BoardConfig {
        uint8_t valid;
        uint8_t value_register;
        uint8_t reading_register;
        uint8_t active_register;
        uint8_t number_of_values;
        float divisor;
        const char *name;
    };

private:
    static constexpr uint8_t OEM_TYPE_EC = 4;
    static constexpr uint8_t OEM_TYPE_PH = 1;
    static constexpr uint8_t OEM_TYPE_DO = 3;
    static constexpr uint8_t OEM_TYPE_TEMP = 5;

    BoardConfig config() {
        switch (type_) {
        case OEM_TYPE_EC: {
            return { true, 0x18, 0x07, 0x06, 3, 100.0f, "EC" };
        }
        case OEM_TYPE_PH: {
            return { true, 0x16, 0x07, 0x06, 1, 1000.0f, "PH" };
        }
        case OEM_TYPE_DO: {
            return { true, 0x22, 0x07, 0x06, 1, 100.0f, "DO" };
        }
        case OEM_TYPE_TEMP: {
            return { true, 0x0E, 0x07, 0x06, 1, 1000.0f, "TEMP" };
        }
        }

        return { false, 0, 0, 0, 0, 1.0f, "<unknown>" };
    }

public:
    AtlasScientificOemBoard(uint8_t address) : address_(address) {
    }

public:
    bool begin() override {
        fk::TwoWire16 d16;

        bus.send(address_, (uint8_t)0x00);
        bus.receive(address_, d16);

        type_ = d16.bytes[1];
        auto version = d16.bytes[0];

        delay(10);

        auto cfg = config();

        if (!cfg.valid) {
            return false;
        }

        auto active = get_register(cfg.active_register);
        delay(10);

        auto irq = get_register(0x04);
        delay(10);

        if (!wake()) {
            return false;
        }

        loginfof("Atlas", "Board type=%d version=%d active=%d irq=%d", type_, version, active, irq);

        return true;
    }

    bool has_reading() {
        return get_register(config().reading_register);
    }

    uint8_t get_register(uint8_t reg) {
        return bus.read(address_, reg);
    }

    bool wake() {
        auto cfg = config();

        bus.write(address_, config().active_register, AtlasHigh);

        delay(10);

        return get_register(cfg.active_register) == 1;
    }

    bool hibernate() {
        auto cfg = config();

        bus.write(address_, config().active_register, AtlasLow);

        delay(10);

        return get_register(cfg.active_register) == 0;
    }

    bool configure() override {
        if (type_ != OEM_TYPE_EC) {
            return true;
        }

        loginfof("Atlas", "Setting EC probe type...");

        uint16_t type = (uint16_t)(0.1 * 100);

        Wire.beginTransmission(address_);
        Wire.write(0x08);
        Wire.write((type >> 8) & 0xff);
        Wire.endTransmission();

        Wire.beginTransmission(address_);
        Wire.write(0x09);
        Wire.write((type) & 0xff);
        Wire.endTransmission();

        union data_t {
            uint8_t bytes[2];
            uint16_t u16;
        };

        data_t data;
        Wire.beginTransmission(address_);
        Wire.write(0x08);
        Wire.endTransmission();

        Wire.requestFrom(address_, 2);
        for (auto i = 2; i > 0; --i) {
            data.bytes[i - 1] = Wire.read();
        }
        Wire.endTransmission();

        float value = data.u16;
        value /= 100.0f;

        loginfof("Atlas", "Current probe type: %f", value);

        delay(100);

        return true;
    }

    bool reading() override {
        auto cfg = config();

        if (!has_reading()) {
            return true;
        }

        if (!bus.send(address_, cfg.value_register)) {
            loginfof("Module", "Atlas(0x%x, %s) Error 2", address_, cfg.name);
            fk_assert(false);
        }

        union data_t {
            uint8_t bytes[4];
            uint32_t u32;
        };

        for (auto i = 0; i < cfg.number_of_values; ++i) {
            fk::TwoWire32 data;

            if (!bus.receive(address_, data)) {
                loginfof("Module", "Atlas(0x%x, %s) Error 1", address_, cfg.name);
                fk_assert(false);
            }

            float value = data.u32;
            value /= cfg.divisor;

            loginfof("Module", "%s: %f (raw=%lu)", cfg.name, value, data.u32);
        }


        return true;
    }

    bool find() override {
        for (auto i = 0; i < 3; ++i) {
            ledsOn();
            delay(100);
            ledsOff();
            delay(100);
        }

        return true;
    }

    bool status() override {
        return true;
    }

    bool info() override {
        return true;
    }

    bool ledsOn() override {
        Wire.beginTransmission(address_);
        Wire.write(0x05);
        Wire.write(0x01);
        Wire.endTransmission();

        return true;
    }

    bool ledsOff() override {
        Wire.beginTransmission(address_);
        Wire.write(0x05);
        Wire.write(0x00);
        Wire.endTransmission();

        return true;
    }
};

class AtlasScientificPrototypeBoard : public AtlasScientificBoard {
private:
    uint8_t address_;
    uint8_t type_;

private:
    static constexpr uint8_t TYPE_EC = 4;
    static constexpr uint8_t TYPE_PH = 1;
    static constexpr uint8_t TYPE_DO = 3;
    static constexpr uint8_t TYPE_TEMP = 5;
    static constexpr uint8_t TYPE_ORP = 2; // Double check this.

private:
    uint8_t readResponse(const char *str, char *buffer, size_t length, uint32_t read_delay = 200) {
        Wire.beginTransmission(address_);
        Wire.write(str);
        Wire.endTransmission();

        delay(read_delay);

        while (true) {
            Wire.requestFrom((uint8_t)address_, 1 + length, (uint8_t)1);

            uint8_t code = Wire.read();
            if (code == ATLAS_RESPONSE_CODE_NOT_READY) {
                delay(100);
                continue;
            }

            uint8_t i = 0;
            while (Wire.available()) {
                uint8_t c = Wire.read();
                if (buffer != nullptr && i < length - 1) {
                    buffer[i++] = c;
                }
                if (c == 0) {
                    Wire.endTransmission();
                    break;
                }
            }
            if (buffer != nullptr) {
                buffer[i] = 0;
            }

            return code;
        }
    }

public:
    AtlasScientificPrototypeBoard(uint8_t address) : address_(address) {
    }

public:
    bool begin() override {
        char buffer[20];
        uint8_t value = readResponse("I", buffer, sizeof(buffer));
        if (value == 0xff) {
            return false;
        }

        if (strstr(buffer, "EC") != nullptr) {
            type_ = TYPE_EC;
        }
        else if (strstr(buffer, "DO") != nullptr) {
            type_ = TYPE_DO;
        }
        else if (strstr(buffer, "ORP") != nullptr) {
            type_ = TYPE_ORP;
        }
        else if (strstr(buffer, "pH") != nullptr) {
            type_ = TYPE_PH;
        }
        else if (strstr(buffer, "RTD") != nullptr) {
            type_ = TYPE_TEMP;
        }
        else {
            return false;
        }

        return true;
    }

    bool configure() override {
        return true;
    }

    bool reading() override {
        char buffer[20];
        uint8_t value = readResponse("R", buffer, sizeof(buffer));
        switch (type_) {
        case TYPE_EC: Serial.print("EC: "); break;
        case TYPE_PH: Serial.print("PH: "); break;
        case TYPE_DO: Serial.print("DO: "); break;
        case TYPE_TEMP: Serial.print("TEMP: "); break;
        case TYPE_ORP: Serial.print("ORP: "); break;
        }
        Serial.println(buffer);
        return value == 0x1;
    }

    bool find() override {
        uint8_t value = readResponse("FIND", nullptr, 0);
        return value == 0x1;
    }

    bool status() override {
        char buffer[20];
        uint8_t value = readResponse("STATUS", buffer, sizeof(buffer));
        Serial.print("test: STATUS: ");
        Serial.println(buffer);
        return value == 0x1;
    }

    bool info() override {
        char buffer[20];
        uint8_t value = readResponse("I", buffer, sizeof(buffer));
        Serial.print("test: INFO: ");
        Serial.println(buffer);
        return value == 0x1;
    }

    bool ledsOn() override {
        uint8_t value = readResponse("L,1", nullptr, 0);
        return value == 0x1;
    }

    bool ledsOff() override {
        uint8_t value = readResponse("L,0", nullptr, 0);
        return value == 0x1;
    }

};

#if defined(FK_ATLAS_OEM)
const uint8_t ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const uint8_t ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x68;
const uint8_t ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x65;
const uint8_t ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x67;
const uint8_t ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x66;
using AtlasBoardType = AtlasScientificOemBoard;
#else
const uint8_t ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const uint8_t ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const uint8_t ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const uint8_t ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const uint8_t ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;
using AtlasBoardType = AtlasScientificPrototypeBoard;
#endif

class Check {
public:
    bool test(uint8_t address, const char *name) {
        AtlasBoardType sensor(address);
        if (!sensor.begin()) {
            if (nullptr != name) {
                loginfof("Atlas", "test: %s FAILED", name);
            }
            return false;
        }
        else {
            if (nullptr != name) {
                loginfof("Atlas", "test: %s PASSED", name);
            }
        }

        if (!sensor.info()) {
            loginfof("Atlas", "test: INFO FAILED");
        }

        if (!sensor.status()) {
            loginfof("Atlas", "test: STATUS FAILED");
        }

        if (!sensor.ledsOn()) {
            loginfof("Atlas", "test: LEDS FAILED");
        }

        sensor.find();

        return true;
    }

    bool ec() {
        return test(ATLAS_SENSOR_EC_DEFAULT_ADDRESS, "EC");
    }

    bool temp() {
        return test(ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS, "TEMP");
    }

    bool ph() {
        return test(ATLAS_SENSOR_PH_DEFAULT_ADDRESS, "PH");
    }

    bool dissolvedOxygen() {
        return test(ATLAS_SENSOR_DO_DEFAULT_ADDRESS, "DO");
    }

    bool orp() {
        return test(ATLAS_SENSOR_ORP_DEFAULT_ADDRESS, "ORP");
    }

    bool scan_entire_bus() {
        Serial.println("test: Scanning entire bus.");

        for (uint8_t i = 128; i > 0; --i) {
            if (test(i, nullptr)) {
                loginfof("Atlas", "Found: %x", i);
            }
        }

        Serial.println();

        return false;
    }

    bool flashMemory() {
        Serial.println("test: Checking flash memory...");

        if (!SerialFlash.begin(fk::FK_ATLAS_PIN_FLASH_CS)) {
            Serial.println("test: Flash memory FAILED");
            return false;
        }

        uint8_t buffer[256];
        SerialFlash.readID(buffer);
        if (buffer[0] == 0) {
            Serial.println("test: Flash memory FAILED");
            return false;
        }

        uint32_t chipSize = SerialFlash.capacity(buffer);
        if (chipSize == 0) {
            Serial.println("test: Flash memory FAILED");
            return false;
        }

        Serial.println("test: Flash memory PASSED");
        return true;
    }

};

void setup() {
    Serial.begin(115200);

    fk::board.disable_everything();

    while (!Serial && millis() < 5000) {
        delay(100);
    }

    auto takeReadings = true;

    while (true) {
        Serial.println("test: Begin");

        fk::board.enable_everything();

        SPI.begin();
        Wire.begin();

        delay(300);

        Check check;

        auto success = true;

        success = check.flashMemory() && success;
        success = check.ec() && success;
        success = check.temp() && success;
        success = check.ph() && success;
        success = check.dissolvedOxygen() && success;
        #ifdef FK_ENABLE_ATLAS_ORP
        success = check.orp() && success;
        #endif

        if (!success) {
            check.scan_entire_bus();
        }

        if (success) {
            loginfof("Atlas", "test: PASSED");
        }
        else {
            loginfof("Atlas", "test: FAILED");
        }

        if (takeReadings && success) {
            AtlasBoardType boards[] = {
                ATLAS_SENSOR_EC_DEFAULT_ADDRESS,
                ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS,
                ATLAS_SENSOR_PH_DEFAULT_ADDRESS,
                ATLAS_SENSOR_DO_DEFAULT_ADDRESS
            };

            auto initialized = true;
            for (auto &board : boards) {
                if (!board.begin()) {
                    initialized = false;
                }
                else {
                    board.configure();
                }
            }

            if (initialized) {
                while (true) {
                    loginfof("Atlas", "-------------------");

                    for (auto &board : boards) {
                        // board.ledsOn();
                        board.reading();
                        // board.ledsOff();
                    }

                    delay(1000);
                }
            }
        }

        Serial.println("test: Done");

        delay(1000);
    }

    delay(100);
}

void loop() {
}
