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
        state = AtlasReaderState::WaitingOnReply;
        postReplyState = AtlasReaderState::LedsOn;
        break;
    }
    case AtlasReaderState::LedsOn: {
        ledsOn();
        state = AtlasReaderState::WaitingOnStatus;
        postReplyState = AtlasReaderState::Blink;
        break;
    }
    case AtlasReaderState::Blink: {
        find();
        state = AtlasReaderState::WaitingOnStatus;
        postReplyState = AtlasReaderState::Sleep;
        break;
    }
    case AtlasReaderState::Sleep: {
        sleep();
        state = AtlasReaderState::Sleeping;
        break;
    }
    case AtlasReaderState::Sleeping: {
        break;
    }
    case AtlasReaderState::Idle: {
        break;
    }
    case AtlasReaderState::Reading: {
        break;
    }
    case AtlasReaderState::WaitingOnStatus: {
        if (nextReadAt > millis()) {
            break;
        }
        if (readReply(nullptr, 0) == ATLAS_RESPONSE_CODE_NOT_READY) {
            nextReadAt = millis() + 100;
            break;
        }
        state = postReplyState;
        break;
    }
    case AtlasReaderState::WaitingOnReply: {
        if (nextReadAt > millis()) {
            break;
        }
        char buffer[20];
        if (readReply(buffer, sizeof(buffer)) == ATLAS_RESPONSE_CODE_NOT_READY) {
            nextReadAt = millis() + 100;
            break;
        }
        state = postReplyState;
        break;
    }
    case AtlasReaderState::Done: {
        break;
    }
    }
    return true;
}

bool AtlasReader::beginReading() {
    // state = AtlasReaderState::Reading;
    // readings = 0;
    return true;
}

bool AtlasReader::hasReading() {
    return false;
}

void AtlasReader::info() {
    sendCommand("I");
}

void AtlasReader::ledsOff() {
    sendCommand("L,0");
}

void AtlasReader::find() {
    sendCommand("FIND");
}

void AtlasReader::ledsOn() {
    sendCommand("L,1");
}

void AtlasReader::sleep() {
    sendCommand("SLEEP");
}

void AtlasReader::read() {
    sendCommand("R");
}

uint8_t AtlasReader::sendCommand(const char *str, uint32_t readDelay) {
    fkprintf("Command(%s, %d)\r\n", str, readDelay);

    bus->beginTransmission(address);
    bus->write(str);
    bus->endTransmission();

    nextReadAt  = millis() + readDelay;
    return ATLAS_RESPONSE_CODE_NOT_READY;
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
        // Sometimes that first byte we read a \0 from the device.
        if (strlen(buffer) > 0) {
            fkprintf("Done '%s' (%d)\r\n", buffer, strlen(buffer));
        }
    }

    return code;
}
