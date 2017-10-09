#include <Arduino.h>
#include <Wire.h>

const uint8_t ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const uint8_t ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const uint8_t ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x65;
const uint8_t ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const uint8_t ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

class AtlasScientificBoard {
private:
    uint8_t address;

private:
    uint8_t readResponse(const char *str, char *buffer, size_t length, uint32_t read_delay = 300) {
        Wire.beginTransmission(address);
        Wire.write(str);
        Wire.endTransmission();

        delay(read_delay);

        Wire.requestFrom((uint8_t)address, length, (uint8_t)1);

        uint8_t code = Wire.read();
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

public:
    AtlasScientificBoard(uint8_t address) {
        this->address = address;
    }

    bool begin() {
        char buffer[20];
        uint8_t value = readResponse("i", buffer, sizeof(buffer));
        return value != 0xff;
    }
};

class Check {
public:
    void setup() {
        Wire.begin();
    }

    void ec() {
        AtlasScientificBoard sensor(ATLAS_SENSOR_EC_DEFAULT_ADDRESS);
        if (!sensor.begin()) {
            Serial.println("test: EC FAILED");
        }
        else {
            Serial.println("test: EC PASSED");
        }
    }

    void temp() {
        AtlasScientificBoard sensor(ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS);
        if (!sensor.begin()) {
            Serial.println("test: TEMP FAILED");
        }
        else {
            Serial.println("test: TEMP PASSED");
        }
    }

    void ph() {
        AtlasScientificBoard sensor(ATLAS_SENSOR_PH_DEFAULT_ADDRESS);
        if (!sensor.begin()) {
            Serial.println("test: PH FAILED");
        }
        else {
            Serial.println("test: PH PASSED");
        }
    }

    void dissolvedOxygen() {
        AtlasScientificBoard sensor(ATLAS_SENSOR_DO_DEFAULT_ADDRESS);
        if (!sensor.begin()) {
            Serial.println("test: DO FAILED");
        }
        else {
            Serial.println("test: DO PASSED");
        }
    }

    void orp() {
        AtlasScientificBoard sensor(ATLAS_SENSOR_ORP_DEFAULT_ADDRESS);
        if (!sensor.begin()) {
            Serial.println("test: ORP FAILED");
        }
        else {
            Serial.println("test: ORP PASSED");
        }
    }

};

void setup() {
    Serial.begin(115200);

    Check check;
    check.setup();

    pinMode(13, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(A4, OUTPUT);
    pinMode(A5, OUTPUT);

    while (!Serial && millis() < 2 * 1000) {
        delay(100);
    }

    while (true) {
        digitalWrite(13, LOW);
        digitalWrite(A3, LOW);
        digitalWrite(A4, LOW);
        digitalWrite(A5, LOW);

        Serial.println("test: Begin");

        check.ec();
        check.temp();
        check.ph();
        check.dissolvedOxygen();
        check.orp();

        Serial.println("test: Done");

        digitalWrite(13, HIGH);
        digitalWrite(A3, HIGH);
        digitalWrite(A4, HIGH);
        digitalWrite(A5, HIGH);

        delay(1000);
    }

    delay(100);
}

void loop() {
}
