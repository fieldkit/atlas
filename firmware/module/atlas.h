#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include <Arduino.h>

#include "sensors.h"

const uint8_t ATLAS_RESPONSE_CODE_NO_DATA = 0xff;
const uint8_t ATLAS_RESPONSE_CODE_NOT_READY = 0xfe;
const uint8_t ATLAS_RESPONSE_CODE_ERROR = 0x2;
const uint8_t ATLAS_RESPONSE_CODE_SUCCESS = 0x1;

enum class AtlasReaderState {
    Start,
    Blink,
    Sleep,
    Sleeping,
    Idle,
    Reading,
    WaitingOnReading,
    Done
};

class AtlasReader : public SensorReader {
private:
    AtlasReaderState state { AtlasReaderState::Start };
    uint32_t nextReadAt { 0 };
    uint8_t readings { 0 };
    TwoWire *bus { nullptr };
    uint8_t address { 0 };

public:
    AtlasReader(TwoWire *theBus, uint8_t theAddress) : bus(theBus), address(theAddress) {
    }

    bool setup() override {
        return true;
    }

    bool tick() override {
        switch (state) {
        case AtlasReaderState::Start: {
            begin();
            state = AtlasReaderState::Blink;
            break;
        }
        case AtlasReaderState::Blink: {
            ledsOn();
            state = AtlasReaderState::Sleep;
            break;
        }
        case AtlasReaderState::Sleep: {
            sleep();
            state = AtlasReaderState::Idle;
            break;
        }
        case AtlasReaderState::Sleeping: 
        case AtlasReaderState::Idle: {
            break;
        }
        case AtlasReaderState::Reading: {
            read();
            state = AtlasReaderState::WaitingOnReading;
            break;
        }
        case AtlasReaderState::WaitingOnReading: {
            if (nextReadAt > millis()) {
                break;
            }
            char buffer[20];
            if (readReply(buffer, sizeof(buffer)) != ATLAS_RESPONSE_CODE_NOT_READY) {
                if (readings < 20) {
                    readings++;
                    state = AtlasReaderState::Reading;
                }
                else {
                    state = AtlasReaderState::Done;
                }
            }
            break;
        }
        case AtlasReaderState::Done: {
            break;
        }
        }
        return true;
    }

    bool beginReading() override {
        state = AtlasReaderState::Reading;
        readings = 0;
        return true;
    }

    bool hasReading() override {
        return false;
    }

private:
    bool begin() {
        char buffer[20];
        uint8_t value = sendCommand("I", buffer, sizeof(buffer));
        return value != 0xff;
    }

    bool ledsOff() {
        return sendCommand("L,0") == 0x1;
    }

    bool ledsOn() {
        sendCommand("FIND");
        return sendCommand("L,1") == 0x1;
    }

    void sleep() {
        sendCommand("SLEEP", nullptr, 0, 0, false);
    }

    void read() {
        sendCommand("R", nullptr, 0, 1000, false);
    }

public:
    uint8_t sendCommand(const char *str, char *buffer = nullptr, size_t length = 0, uint32_t readDelay = 300, bool sync = true) {
        fkprintf("sendCommand(%s, %x, %d, %d, %d)\r\n", str, buffer, length, readDelay, sync);

        bus->beginTransmission(address);
        bus->write(str);
        bus->endTransmission();

        if (!sync ) {
            nextReadAt  = millis() + readDelay;
            return ATLAS_RESPONSE_CODE_NOT_READY;
        }
        else {
            delay(readDelay);
        }

        while (true) {
            uint8_t code = readReply(buffer, length);
            if (code == ATLAS_RESPONSE_CODE_NOT_READY) {
                delay(100);
            }
            else {
                return code;
            }
        }
    }

    uint8_t readReply(char *buffer, size_t length) {
        bus->requestFrom((uint8_t)address, 1 + length, (uint8_t)1);

        uint8_t code = bus->read();
        if (code == ATLAS_RESPONSE_CODE_NOT_READY) {
            return ATLAS_RESPONSE_CODE_NOT_READY;
        }

        uint8_t i = 0;
        while (bus->available()) {
            uint8_t c = bus->read();
            if (buffer != nullptr && i < length - 1) {
                buffer[i++] = c;
            }
            if (c == 0) {
                bus->endTransmission();
                break;
            }
        }
        if (buffer != nullptr) {
            buffer[i] = 0;
            fkprintf("Done '%s' (%d)\r\n", buffer, i);
        }

        return code;
    }

};

#endif
