#include "atlas.h"
#include "logging.h"

AtlasReader::AtlasReader(TwoWire *theBus, uint8_t theAddress)
    : bus(theBus), address(theAddress) {
}

bool AtlasReader::setup() {
    return true;
}

bool AtlasReader::tick() {
    if (nextCheckAt > 0) {
        if (nextCheckAt > millis()) {
            return false;
        }

        nextCheckAt = 0;
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
        postReplyState = AtlasReaderState::Sleep;

        // Sleep is annoying because some of the modules seem to awaken from
        // sleep if we're talking to other modules. So this works best if we
        // issue SLEEP to all of them at once w/o needing to talk to the others
        // concurrently.
        // Also, the RTD sensor seems to just awake from sleep randomly.
        nextCheckAt = millis() + ATLAS_DEFAULT_DELAY_SLEEP;
        break;
    }
    case AtlasReaderState::Sleep: {
        sendCommand("SLEEP");
        state = AtlasReaderState::Sleeping;
        break;
    }
    case AtlasReaderState::TakeReading: {
        sendCommand("R", ATLAS_DEFAULT_DELAY_COMMAND_READ);
        state = AtlasReaderState::WaitingOnReply;
        postReplyState = AtlasReaderState::ParseReading;
        break;
    }
    case AtlasReaderState::ParseReading: {
        state = AtlasReaderState::Sleep;
        break;
    }
    case AtlasReaderState::WaitingOnEmptyReply: {
        if (readReply(nullptr, 0) == AtlasResponseCode::NotReady) {
            nextCheckAt = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
            break;
        }
        state = postReplyState;
        break;
    }
    case AtlasReaderState::WaitingOnReply: {
        char buffer[ATLAS_MAXIMUM_COMMAND_LENGTH];
        if (readReply(buffer, sizeof(buffer)) == AtlasResponseCode::NotReady) {
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
    state = AtlasReaderState::TakeReading;
    return true;
}

size_t AtlasReader::numberOfReadingsReady() const {
    return numberOfValues;
}

bool AtlasReader::isIdle() const {
    return state == AtlasReaderState::Idle || state == AtlasReaderState::Sleeping;
}

AtlasResponseCode AtlasReader::sendCommand(const char *str, uint32_t readDelay) {
    fkprintln("Atlas(0x%x) <- ('%s', %d))", address, str, readDelay);

    bus->beginTransmission(address);
    bus->write(str);
    bus->endTransmission();

    nextCheckAt  = millis() + readDelay;
    return AtlasResponseCode::NotReady;
}

static AtlasSensorType getSensorType(const char *buffer) {
    if (strstr(buffer, "RTD") != nullptr)  return AtlasSensorType::Temp;
    if (strstr(buffer, "pH") != nullptr)  return AtlasSensorType::Ph;
    if (strstr(buffer, "DO") != nullptr)  return AtlasSensorType::Do;
    if (strstr(buffer, "ORP") != nullptr)  return AtlasSensorType::Orp;
    if (strstr(buffer, "EC") != nullptr)  return AtlasSensorType::Ec;
    return AtlasSensorType::Unknown;
}

AtlasResponseCode AtlasReader::readReply(char *buffer, size_t length) {
    bus->requestFrom(address, 1 + length, (uint8_t)1);

    auto code = static_cast<AtlasResponseCode>(bus->read());
    if (code == AtlasResponseCode::NotReady) {
        return AtlasResponseCode::NotReady;
    }

    size_t i = 0;
    while (bus->available()) {
        auto c = bus->read();
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
        fkprintln("Atlas(0x%x) -> ('%s')", address, buffer, strlen(buffer));

        if (type == AtlasSensorType::Unknown) {
            type = getSensorType(buffer);
        }

        if (postReplyState == AtlasReaderState::ParseReading) {
            String line = buffer;
            int16_t position = 0;

            numberOfValues = 0;

            while (position < (int16_t)line.length()) {
                int16_t index = line.indexOf(',', position);
                if (index < 0) {
                    index = line.length();
                }
                if (numberOfValues < ATLAS_MAXIMUM_NUMBER_OF_VALUES)  {
                    if (index > position) {
                        String part = line.substring(position, index);
                        values[numberOfValues++] = part.toFloat();
                        position = index + 1;
                    }
                }
                else {
                    fkprintln("Error: Too many values");
                    break;
                }
            }
        }
    }

    return code;
}

size_t AtlasReader::readAll(float *values) {
    for (size_t i = 0; i < numberOfValues; ++i) {
        *values++ = this->values[i];
    }
    size_t number = numberOfValues;
    numberOfValues = 0;
    return number;
}
