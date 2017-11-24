#include "atlas.h"
#include "logging.h"

AtlasReader::AtlasReader(TwoWire *theBus, uint8_t theAddress)
    : bus(theBus), address(theAddress) {
}

bool AtlasReader::setup() {
    return true;
}

bool AtlasReader::tick() {
    switch (state) {
    case AtlasReaderState::Start: {
        info();
        state = AtlasReaderState::Blink;
        break;
    }
    case AtlasReaderState::Blink: {
        ledsOn();
        find();
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

bool AtlasReader::beginReading() {
    state = AtlasReaderState::Reading;
    readings = 0;
    return true;
}

bool AtlasReader::hasReading() {
    return false;
}

bool AtlasReader::info() {
    char buffer[20];
    uint8_t value = sendCommand("I", buffer, sizeof(buffer));
    return value == ATLAS_RESPONSE_CODE_SUCCESS;
}

bool AtlasReader::ledsOff() {
    return sendCommand("L,0") == ATLAS_RESPONSE_CODE_SUCCESS;
}

bool AtlasReader::find() {
    return sendCommand("FIND") == ATLAS_RESPONSE_CODE_SUCCESS;
}

bool AtlasReader::ledsOn() {
    return sendCommand("L,1") == ATLAS_RESPONSE_CODE_SUCCESS;
}

void AtlasReader::sleep() {
    sendCommand("SLEEP", nullptr, 0, 0, false);
}

void AtlasReader::read() {
    sendCommand("R", nullptr, 0, 1000, false);
}

uint8_t AtlasReader::sendCommand(const char *str, char *buffer, size_t length, uint32_t readDelay, bool sync) {
    fkprintf("Command(%s, %x, %d, %d, %d)\r\n", str, buffer, length, readDelay, sync);

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

uint8_t AtlasReader::readReply(char *buffer, size_t length) {
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
