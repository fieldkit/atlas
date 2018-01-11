#include "atlas.h"
#include "debug.h"

namespace fk {

constexpr char Log[] = "Atlas";

AtlasReader::AtlasReader(TwoWire *theBus, uint8_t theAddress)
    : bus(theBus), address(theAddress) {
}

bool AtlasReader::setup() {
    return true;
}

void AtlasReader::sleep() {
    switch (state) {
    case AtlasReaderState::WantSleep: {
        state = AtlasReaderState::Sleep;
        break;
    }
    default: {
        state = AtlasReaderState::WantSleep;
        break;
    }
    }
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

TickSlice AtlasReader::tick() {
    if (nextCheckAt > 0) {
        if (nextCheckAt > millis()) {
            return TickSlice{};
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
        postReplyState = AtlasReaderState::WantSleep;
        break;
    }
    case AtlasReaderState::WantSleep: {
        return TickSlice{ [&]() {
                sleep();
            }
        };
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
    return TickSlice{};
}

AtlasResponseCode AtlasReader::sendCommand(const char *str, uint32_t readDelay) {
    debugfpln(Log, "Atlas(0x%x) <- ('%s', %lu))", address, str, readDelay);

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
        debugfpln(Log, "Atlas(0x%x) -> ('%s')", address, buffer);

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
                    debugfpln(Log, "Error: Too many values");
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

}
