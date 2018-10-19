#include "atlas.h"
#include "debug.h"
#include "configuration.h"

namespace fk {

constexpr char Log[] = "Atlas";

AtlasReader::AtlasReader(TwoWireBus &bus, uint8_t theAddress)
    : bus(&bus), address(theAddress) {
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

bool AtlasReader::beginReading(bool sleep) {
    sleepAfter = sleep;
    state = AtlasReaderState::ApplyCompensation;
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
        for (size_t i = 0; i < ATLAS_MAXIMUM_NUMBER_OF_VALUES; ++i) {
            values[i] = 0.0f;
        }
        break;
    }
    case AtlasReaderState::LedsOn: {
        if (fk_uptime() > configuration.common.leds.disable_after) {
            sendCommand("L,0");
        }
        else {
            sendCommand("L,1");
        }
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
        postReplyState = AtlasReaderState::QueryParameters;
        break;
    }
    case AtlasReaderState::QueryParameters: {
        switch (type) {
        case AtlasSensorType::Ec: {
            sendCommand("O,?");
            state = AtlasReaderState::WaitingOnReply;
            postReplyState = AtlasReaderState::ConfigureParameter;
            break;
        }
        default: {
            state = AtlasReaderState::WantSleep;
            break;
        }
        }
        break;
    }
    case AtlasReaderState::ConfigureParameter: {
        if (parameter < 4) {
            switch (parameter) {
            case 0: sendCommand("O,EC,1"); break;
            case 1: sendCommand("O,TDS,1"); break;
            case 2: sendCommand("O,S,1"); break;
            case 3: sendCommand("O,SG,1"); break;
            }
            state = AtlasReaderState::WaitingOnEmptyReply;
            postReplyState = AtlasReaderState::ConfigureParameter;
            parameter++;
        }
        else {
            #if defined(FK_ATLAS_HARD_CODED_PROBE_TYPE)
            state = AtlasReaderState::QueryProbeType;
            #else
            state = AtlasReaderState::WantSleep;
            #endif
        }
        break;
    }
    #if defined(FK_ATLAS_HARD_CODED_PROBE_TYPE)
    case AtlasReaderState::QueryProbeType: {
        switch (type) {
        case AtlasSensorType::Ec: {
            sendCommand("K,?");
            state = AtlasReaderState::WaitingOnReply;
            postReplyState = AtlasReaderState::ConfigureProbeType;
            break;
        }
        default: {
            state = AtlasReaderState::WantSleep;
            break;
        }
        }
        break;
    }
    case AtlasReaderState::ConfigureProbeType: {
        sendCommand("K," FK_ATLAS_HARD_CODED_PROBE_TYPE);
        state = AtlasReaderState::WaitingOnEmptyReply;
        postReplyState = AtlasReaderState::WantSleep;
        break;
    }
    #endif
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
    case AtlasReaderState::ApplyCompensation: {
        if (compensation) {
            // NOTE: I wish we could use RT,n, need newer firmware though.
            switch (type) {
            case AtlasSensorType::Ph:
            case AtlasSensorType::Do:
            case AtlasSensorType::Ec: {
                char command[20];
                snprintf(command, sizeof(buffer), "T,%f", compensation.temperature);
                sendCommand(command, ATLAS_DEFAULT_DELAY_COMMAND_READ);
                state = AtlasReaderState::WaitingOnEmptyReply;
                postReplyState = AtlasReaderState::TakeReading;
                break;
            }
            default: {
                state = AtlasReaderState::TakeReading;
                break;
            }
            }
        }
        else {
            state = AtlasReaderState::TakeReading;
        }
        break;
    }
    case AtlasReaderState::TakeReading: {
        sendCommand("R", ATLAS_DEFAULT_DELAY_COMMAND_READ);
        state = AtlasReaderState::WaitingOnReply;
        postReplyState = AtlasReaderState::ParseReading;
        break;
    }
    case AtlasReaderState::ParseReading: {
        if (sleepAfter) {
            state = AtlasReaderState::Sleep;
        }
        else {
            state = AtlasReaderState::Idle;
        }
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

AtlasResponseCode AtlasReader::singleCommand(const char *command) {
    postReplyState = AtlasReaderState::Sleep;
    state = AtlasReaderState::WaitingOnReply;
    return sendCommand(command, 300);
}

AtlasResponseCode AtlasReader::sendCommand(const char *str, uint32_t readDelay) {
    loginfof(Log, "Atlas(0x%x, %s) <- ('%s', %lu))", address, typeName(), str, readDelay);

    bus->send(address, str);

    nextCheckAt  = millis() + readDelay;
    return AtlasResponseCode::NotReady;
}

static AtlasSensorType getSensorType(const char *buffer) {
    if (strstr(buffer, "RTD") != nullptr) return AtlasSensorType::Temp;
    if (strstr(buffer, "pH") != nullptr) return AtlasSensorType::Ph;
    if (strstr(buffer, "DO") != nullptr) return AtlasSensorType::Do;
    if (strstr(buffer, "ORP") != nullptr) return AtlasSensorType::Orp;
    if (strstr(buffer, "EC") != nullptr) return AtlasSensorType::Ec;
    return AtlasSensorType::Unknown;
}

const char *AtlasReader::typeName() {
    switch (type) {
    case AtlasSensorType::Unknown: return "Unknown";
    case AtlasSensorType::Ph: return "PH";
    case AtlasSensorType::Ec: return "EC";
    case AtlasSensorType::Orp: return "ORP";
    case AtlasSensorType::Do: return "DO";
    case AtlasSensorType::Temp: return "TEMP";
    default:
        return "Invalid";
    }
}

AtlasResponseCode AtlasReader::readReply(char *buffer, size_t length) {
    bus->requestFrom(address, 1 + length, (uint8_t)1);

    auto code = static_cast<AtlasResponseCode>(bus->read());
    if (code == AtlasResponseCode::NotReady) {
        return AtlasResponseCode::NotReady;
    }
    if (code == AtlasResponseCode::NoData) {
        if (buffer != nullptr) {
            loginfof(Log, "Atlas(0x%x, %s) -> (code=0x%x)", address, typeName(), code);
            return AtlasResponseCode::NotReady;
        }
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
        loginfof(Log, "Atlas(0x%x, %s) -> ('%s') (code=0x%x)", address, typeName(), buffer, code);

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
                    loginfof(Log, "Error: Too many values");
                    break;
                }
            }

            if (numberOfValues == 0) {
                loginfof(Log, "No values, retry?");
                tries++;
            }
            else {
                tries = 0;
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
