#include <Arduino.h>
#include <Wire.h>

class AtlasScientificBoard {
private:
    uint8_t address;

private:
    uint8_t read8(const char *str) {
        Wire.beginTransmission(address);
        Wire.write(str);
        Wire.endTransmission();

        delay(300);

        Wire.requestFrom((uint8_t)address, 20, (uint8_t)1);

        uint8_t code = Wire.read();
        uint8_t buffer[20] = { 0 };
        uint8_t i = 0;

        while (Wire.available()) {
            uint8_t c = Wire.read();
            buffer[i++] = c;
            if (c == 0) {
                Wire.endTransmission();
                break;
            }
        }

        if (i > 0) {
            Serial.println((const char *)buffer);
        }

        return code;
    }

public:
    AtlasScientificBoard(uint8_t address) {
        this->address = address;
    }

    bool begin() {
        uint8_t value = read8("i");
        return value != 0xff;
    }
};

class Check {
public:
    void setup() {
        Wire.begin();
    }

    void orp() {
        AtlasScientificBoard sensor(0x62);
        if (!sensor.begin()) {
            Serial.println("test: ORP FAILED");
        }
        else {
            Serial.println("test: ORP PASSED");
        }
    }

    void ec() {
        AtlasScientificBoard sensor(0x64);
        if (!sensor.begin()) {
            Serial.println("test: EC FAILED");
        }
        else {
            Serial.println("test: EC PASSED");
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
