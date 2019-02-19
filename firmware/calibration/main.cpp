#include <Arduino.h>
#include <Wire.h>
#include <SerialFlash.h>

#include "debug.h"
#include "board_definition.h"
#include "two_wire.h"

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
    virtual bool raw(String command) = 0;
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
        Wire.beginTransmission(address_);
        Wire.write(0x00);
        Wire.endTransmission();

        Wire.requestFrom(address_, 2);

        type_ = Wire.read();
        auto version = Wire.read(); // Firmware

        Wire.endTransmission();
        delay(50);

        auto cfg = config();

        if (!cfg.valid) {
            return false;
        }

        auto active = get_register(cfg.active_register);
        delay(50);

        auto irq = get_register(0x04);
        delay(50);

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
        Wire.beginTransmission(address_);
        Wire.write(reg);
        Wire.endTransmission();

        Wire.requestFrom(address_, 1);
        auto value = Wire.read();
        Wire.endTransmission();

        return value;
    }

    bool wake() {
        auto cfg = config();

        Wire.beginTransmission(address_);
        Wire.write(cfg.active_register);
        Wire.write(0x01);
        Wire.endTransmission();

        delay(100);

        return get_register(cfg.active_register) == 1;
    }

    bool hibernate() {
        auto cfg = config();

        Wire.beginTransmission(address_);
        Wire.write(cfg.active_register);
        Wire.write(0x00);
        Wire.endTransmission();

        delay(100);

        return get_register(cfg.active_register) == 0;
    }

    bool raw(String command) override {
        switch (type_) {
        case OEM_TYPE_DO: {
            if (command == "cal,clear") {
                Wire.beginTransmission(address_);
                Wire.write(0x08);
                Wire.write(1);
                Wire.endTransmission();

                return true;
            }
            if (command == "cal") {
                return calibrate_do(2);
            }
            if (command == "cal,0") {
                return calibrate_do(3);
            }
            if (command == "cal,?") {
                auto crr = get_register(0x08);
                auto ccr = get_register(0x09);
                loginfof("Atlas", "CRR: %d CCR: %d", crr, ccr);
                return true;
            }
            break;
        }
        case OEM_TYPE_PH: {
            if (command == "cal,clear") {
                Wire.beginTransmission(address_);
                Wire.write(0x0c);
                Wire.write(1);
                Wire.endTransmission();

                return true;
            }
            if (command == "cal,4") {
                return calibrate_ph(4.0f, 2);
            }
            if (command == "cal,7") {
                return calibrate_ph(7.0f, 3);
            }
            if (command == "cal,10") {
                return calibrate_ph(10.0f, 4);
            }
            if (command == "cal,?") {
                auto crr = get_register(0x0c);
                auto ccr = get_register(0x0d);
                loginfof("Atlas", "CRR: %d CCR: %d", crr, ccr);
                return true;
            }
            break;
        }
        case OEM_TYPE_EC: {
            if (command == "cal,clear") {
                Wire.beginTransmission(address_);
                Wire.write(0x0e);
                Wire.write(1);
                Wire.endTransmission();

                return true;
            }
            if (command == "k,?") {
                Wire.beginTransmission(address_);
                Wire.write(0x08);
                Wire.endTransmission();
                Wire.requestFrom(address_, 2);
                auto msb = Wire.read();
                auto lsb = Wire.read();
                auto value = (msb << 8) | lsb;
                loginfof("Atlas", "k = %f", value / 100.0f);
                return true;
            }
            if (command == "k,1.0") {
                return set_probe_type(1.0f);
            }
            if (command == "k,0.1") {
                return set_probe_type(0.1f);
            }
            if (command == "cal,dry") {
                return calibrate_ec(0.0f, 2);
            }
            if (command == "cal,84") {
                return calibrate_ec(84.0f, 3);
            }
            if (command == "cal,1413") {
                return calibrate_ec(1413.0f, 3);
            }
            if (command == "cal,?") {
                auto crr = get_register(0x0e);
                auto ccr = get_register(0x0f);
                loginfof("Atlas", "CRR: %d CCR: %d", crr, ccr);
                return true;
            }
            break;
        }
        }

        loginfof("Atlas", "Unknown command");

        return false;
    }

    bool calibrate_do(uint8_t kind) {
        Wire.beginTransmission(address_);
        Wire.write(0x08);
        Wire.write(kind);
        Wire.endTransmission();

        while (true) {
            auto crr = get_register(0x08);
            auto ccr = get_register(0x09);
            loginfof("Atlas", "CRR: %d CCR: %d", crr, ccr);
            if (crr == 0) {
                break;
            }
            delay(500);
        }
        return true;
    }

    bool calibrate_ec(float ec, uint8_t kind) {
        auto scaled = (uint32_t)(ec * 100.0f);

        Wire.beginTransmission(address_);
        Wire.write(0x0a);
        Wire.write((scaled >> 24) & 0xff);
        Wire.write((scaled >> 16) & 0xff);
        Wire.write((scaled >> 8) & 0xff);
        Wire.write((scaled) & 0xff);
        Wire.endTransmission();

        loginfof("Atlas", "CValue: %x", scaled);
        loginfof("Atlas", "CRR: %d (writing %d)", get_register(0x0e), kind);

        Wire.beginTransmission(address_);
        Wire.write(0x0e);
        Wire.write(kind);
        Wire.endTransmission();

        while (true) {
            auto crr = get_register(0x0e);
            auto ccr = get_register(0x0f);
            loginfof("Atlas", "CRR: %d CCR: %d", crr, ccr);
            if (crr == 0) {
                break;
            }
            delay(500);
        }

        return true;
    }

    bool calibrate_ph(float ph, uint8_t kind) {
        auto scaled = (uint32_t)(ph * 1000.0f);

        Wire.beginTransmission(address_);
        Wire.write(0x08);
        Wire.write((scaled >> 24) & 0xff);
        Wire.write((scaled >> 16) & 0xff);
        Wire.write((scaled >> 8) & 0xff);
        Wire.write((scaled) & 0xff);
        Wire.endTransmission();

        loginfof("Atlas", "CValue: %x", scaled);
        loginfof("Atlas", "CRR: %d (writing %d)", get_register(0x0c), kind);

        Wire.beginTransmission(address_);
        Wire.write(0x0c);
        Wire.write(kind);
        Wire.endTransmission();

        while (true) {
            auto crr = get_register(0x0c);
            auto ccr = get_register(0x0d);
            loginfof("Atlas", "CRR: %d CCR: %d", crr, ccr);
            if (crr == 0) {
                break;
            }
            delay(500);
        }

        return true;
    }

    bool configure() override {
        if (type_ == OEM_TYPE_EC) {
            return set_probe_type(0.1f);
        }

        return true;
    }

    bool set_probe_type(float k) {
        auto type = (uint16_t)(k * 100.0f);

        Wire.beginTransmission(address_);
        Wire.write(0x08);
        Wire.write((type >> 8) & 0xff);
        Wire.write((type) & 0xff);
        Wire.endTransmission();

        Wire.beginTransmission(address_);
        Wire.write(0x08);
        Wire.endTransmission();
        Wire.requestFrom(address_, 2);
        auto msb = Wire.read();
        auto lsb = Wire.read();
        auto value = (msb << 8) | lsb;
        loginfof("Atlas", "Configured k = %f (%f)", k, value / 100.0f);

        delay(100);

        return true;
    }

    bool reading() override {
        auto cfg = config();

        if (!has_reading()) {
            return true;
        }

        Wire.beginTransmission(address_);
        Wire.write(cfg.value_register);
        Wire.endTransmission();

        union data_t {
            uint8_t bytes[4];
            uint32_t u32;
        };

        for (auto i = 0; i < cfg.number_of_values; ++i) {
            data_t data;

            Wire.requestFrom(address_, 4);
            for (auto i = 4; i > 0; --i) {
                data.bytes[i - 1] = Wire.read();
            }
            Wire.endTransmission();

            float value = data.u32;
            value /= cfg.divisor;

            loginfof("Module", "%s: %f (raw=%lu) (div=%f)", cfg.name, value, data.u32, cfg.divisor);
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

    bool raw(String command) override {
        char buffer[20];
        uint8_t value = readResponse(command.c_str(), buffer, sizeof(buffer));
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
    void setup() {
        Wire.begin();

        //  TODO: Investigate. I would see hangs if I used a slower speed.
        // Wire.setClock(400000);

        fk::board.disable_everything();

        delay(1000);

        fk::board.enable_everything();

        delay(1000);
    }

    bool test(uint8_t address, const char *name) {
        AtlasBoardType sensor(address);
        if (!sensor.begin()) {
            loginfof("Atlas", "test: %s FAILED", name);
            return false;
        }
        else {
            loginfof("Atlas", "test: %s PASSED", name);
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

    bool flashMemory() {
        Serial.println("test: Checking flash memory...");

        if (!SerialFlash.begin(fk::board.flash_cs())) {
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

class Repl {
private:
    bool initialized{ false };
    AtlasBoardType boards[4] = {
        ATLAS_SENSOR_EC_DEFAULT_ADDRESS,
        ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS,
        ATLAS_SENSOR_PH_DEFAULT_ADDRESS,
        ATLAS_SENSOR_DO_DEFAULT_ADDRESS
    };
    AtlasBoardType *active{ nullptr };

public:
    Repl() {
    }

public:
    void begin() {
        for (auto &board : boards) {
            if (!board.begin()) {
                initialized = false;
            }
            else {
                board.configure();
            }
        }
    }

    void execute(String &c) {
        if (c.equals("ph")) {
            Serial.println("Selected pH");
            active = &boards[2];
            return;
        }

        if (c.equals("temp")) {
            Serial.println("Selected Temp");
            active = &boards[1];
            return;
        }

        if (c.equals("ec")) {
            Serial.println("Selected EC");
            active = &boards[0];
            return;
        }

        if (c.equals("do")) {
            Serial.println("Selected DO");
            active = &boards[3];
            return;
        }

        if (active != nullptr) {
            if (c.equals("")) {
                active->reading();
            }
            else {
                active->raw(c);
            }
        }
        else {
            Serial.println("No active board.");
        }
    }

};

void setup() {
    Serial.begin(115200);

    Check check;
    check.setup();

    while (!Serial) {
        delay(100);
    }

    auto takeReadings = true;

    while (true) {
        Serial.println("calibration: Begin");

        auto success = true;

        success = check.flashMemory() || success;
        success = check.ec() || success;
        success = check.temp() || success;
        success = check.ph() || success;
        success = check.dissolvedOxygen();
        #ifdef FK_ENABLE_ATLAS_ORP
        success = check.orp() || success;
        #endif

        if (takeReadings && success) {
            Repl repl;

            auto initialized = true;
            repl.begin();

            if (initialized) {
                String command = "";

                while (true) {
                    if (Serial.available() > 0) {
                        auto incoming = Serial.read();

                        if (incoming == '\r') {
                            command.trim();

                            Serial.println();

                            repl.execute(command);

                            command = "";
                        }
                        else {
                            command += (char)incoming;
                            Serial.print((char)incoming);
                        }
                    }
                }
            }
        }

        Serial.println("calibration: Done");

        fk::board.disable_everything();
        delay(5000);
        fk::board.enable_everything();
        delay(1000);
    }

    delay(100);
}

void loop() {
}
