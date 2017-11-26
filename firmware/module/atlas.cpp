#include "atlas.h"
#include "logging.h"

AtlasReader::AtlasReader(TwoWire *theBus, uint8_t theAddress)
    : bus(theBus), address(theAddress) {
}

bool AtlasReader::setup() {
    return true;
}

bool AtlasReader::tick() {
    if (nextCheckAt > 0 && nextCheckAt > millis()) {
        return false;
    }

    switch (state) {
    case AtlasReaderState::Start: {
        sendCommand("I");
        state = AtlasReaderState::WaitingOnReply;
        postReplyState = AtlasReaderState::LedsOn;
        break;
    }
    case AtlasReaderState::LedsOn: {
        sendCommand("L,1");
        state = AtlasReaderState::WaitingOnEmptyReply;
        postReplyState = AtlasReaderState::Status;
        break;
    }
    case AtlasReaderState::Status: {
        sendCommand("STATUS");
        state = AtlasReaderState::WaitingOnReply;
        postReplyState = AtlasReaderState::Blink;
        break;
    }
    case AtlasReaderState::Blink: {
        sendCommand("FIND");
        state = AtlasReaderState::WaitingOnEmptyReply;

        // Sleep is annoying because some of the modules seem to awaken from
        // sleep if we're talking to other modules. So this works best if we
        // issue SLEEP to all of them at once w/o needing to talk to the others
        // concurrently.
        // Also, the RTD sensor seems to just awake from sleep randomly.
        postReplyState = AtlasReaderState::Sleep;
        nextCheckAt = millis() + ATLAS_DEFAULT_DELAY_SLEEP;
        break;
    }
    case AtlasReaderState::Sleep: {
        sendCommand("SLEEP");
        state = AtlasReaderState::Sleeping;
        break;
    }
    case AtlasReaderState::Reading: {
        break;
    }
    case AtlasReaderState::WaitingOnEmptyReply: {
        if (readReply(nullptr, 0) == ATLAS_RESPONSE_CODE_NOT_READY) {
            nextCheckAt = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
            break;
        }
        state = postReplyState;
        break;
    }
    case AtlasReaderState::WaitingOnReply: {
        char buffer[20];
        if (readReply(buffer, sizeof(buffer)) == ATLAS_RESPONSE_CODE_NOT_READY) {
            nextCheckAt = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
            break;
        }
        state = postReplyState;
        break;
    }
    case AtlasReaderState::Sleeping: {
        break;
    }
    case AtlasReaderState::Idle: {
        break;
    }
    }
    return true;
}

bool AtlasReader::beginReading() {
    return true;
}

bool AtlasReader::hasReading() const {
    return false;
}

bool AtlasReader::isIdle() const {
    return state == AtlasReaderState::Idle || state == AtlasReaderState::Sleeping;
}

uint8_t AtlasReader::sendCommand(const char *str, uint32_t readDelay) {
    fkprintf("Atlas(%x)(%s, %d)\r\n", address, str, readDelay);

    bus->beginTransmission(address);
    bus->write(str);
    bus->endTransmission();

    nextCheckAt  = millis() + readDelay;
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
        fkprintf("Done(%x) '%s' (%d)\r\n", address, buffer, strlen(buffer));
    }

    return code;
}
