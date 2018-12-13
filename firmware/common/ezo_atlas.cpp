#include "ezo_atlas.h"
#include "debug.h"
#include "configuration.h"
#include "atlas_configuration.h"

namespace fk {

constexpr char Log[] = "Atlas";

EzoAtlas::EzoAtlas(TwoWireBus &bus, uint8_t theAddress)
    : bus_(&bus), address_(theAddress) {
}

bool EzoAtlas::setup() {
    return true;
}

void EzoAtlas::sleep() {
    switch (state_) {
    case AtlasReaderState::WantSleep: {
        state_ = AtlasReaderState::Sleep;
        break;
    }
    default: {
        state_ = AtlasReaderState::WantSleep;
        break;
    }
    }
}

bool EzoAtlas::beginReading(bool sleep) {
    sleepAfter_ = sleep;
    state_ = AtlasReaderState::ApplyCompensation;
    return true;
}

size_t EzoAtlas::numberOfReadingsReady() const {
    return numberOfValues_;
}

bool EzoAtlas::isIdle() const {
    return state_ == AtlasReaderState::Idle || state_ == AtlasReaderState::Sleeping;
}

TickSlice EzoAtlas::tick() {
    if (nextCheckAt_ > 0) {
        if (nextCheckAt_ > millis()) {
            return TickSlice{};
        }

        nextCheckAt_ = 0;
    }

    switch (state_) {
    case AtlasReaderState::Start: {
        sendCommand("I");
        state_ = AtlasReaderState::WaitingOnReply;
        postReplyState_ = AtlasReaderState::LedsOn;
        for (size_t i = 0; i < ATLAS_MAXIMUM_NUMBER_OF_VALUES; ++i) {
            values_[i] = 0.0f;
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
        state_ = AtlasReaderState::WaitingOnEmptyReply;
        postReplyState_ = AtlasReaderState::Status;
        break;
    }
    case AtlasReaderState::Status: {
        sendCommand("STATUS");
        state_ = AtlasReaderState::WaitingOnReply;
        postReplyState_ = AtlasReaderState::Blink;
        break;
    }
    case AtlasReaderState::Blink: {
        sendCommand("Find");
        state_ = AtlasReaderState::WaitingOnEmptyReply;
        postReplyState_ = AtlasReaderState::QueryProtocolLock;
        break;
    }
    case AtlasReaderState::QueryProtocolLock: {
        sendCommand("Plock,?");
        state_ = AtlasReaderState::WaitingOnReply;
        postReplyState_ = AtlasReaderState::LockProtocol;
        break;
    }
    case AtlasReaderState::LockProtocol: {
        sendCommand("Plock,1");
        state_ = AtlasReaderState::WaitingOnEmptyReply;
        postReplyState_ = AtlasReaderState::QueryParameters;
        break;
    }
    case AtlasReaderState::QueryParameters: {
        switch (type_) {
        case AtlasSensorType::Ec: {
            sendCommand("O,?");
            state_ = AtlasReaderState::WaitingOnReply;
            postReplyState_ = AtlasReaderState::ConfigureParameter;
            parameter_ = 0;
            break;
        }
        default: {
            state_ = AtlasReaderState::WantSleep;
            break;
        }
        }
        break;
    }
    case AtlasReaderState::ConfigureParameter: {
        if (parameter_ < 4) {
            switch (parameter_) {
            case 0: sendCommand("O,EC,1"); break;
            case 1: sendCommand("O,TDS,1"); break;
            case 2: sendCommand("O,S,1"); break;
            case 3: sendCommand("O,SG,1"); break;
            }
            state_ = AtlasReaderState::WaitingOnEmptyReply;
            postReplyState_ = AtlasReaderState::ConfigureParameter;
        }
        else {
            #if defined(FK_ATLAS_HARD_CODED_PROBE_TYPE)
            state_ = AtlasReaderState::QueryProbeType;
            #else
            state_ = AtlasReaderState::WantSleep;
            #endif
        }
        break;
    }
    #if defined(FK_ATLAS_HARD_CODED_PROBE_TYPE)
    case AtlasReaderState::QueryProbeType: {
        switch (type_) {
        case AtlasSensorType::Ec: {
            sendCommand("K,?");
            state_ = AtlasReaderState::WaitingOnReply;
            postReplyState_ = AtlasReaderState::ConfigureProbeType;
            break;
        }
        default: {
            state_ = AtlasReaderState::WantSleep;
            break;
        }
        }
        break;
    }
    case AtlasReaderState::ConfigureProbeType: {
        sendCommand("K," FK_ATLAS_HARD_CODED_PROBE_TYPE);
        state_ = AtlasReaderState::WaitingOnEmptyReply;
        postReplyState_ = AtlasReaderState::WantSleep;
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
        state_ = AtlasReaderState::Sleeping;
        break;
    }
    case AtlasReaderState::ApplyCompensation: {
        if (compensation_) {
            // NOTE: I wish we could use RT,n, need newer firmware though.
            switch (type_) {
            case AtlasSensorType::Ph:
            case AtlasSensorType::Do:
            case AtlasSensorType::Ec: {
                char command[20];
                snprintf(command, sizeof(buffer_), "T,%f", compensation_.temperature);
                sendCommand(command, ATLAS_DEFAULT_DELAY_COMMAND_READ);
                state_ = AtlasReaderState::WaitingOnEmptyReply;
                postReplyState_ = AtlasReaderState::TakeReading;
                break;
            }
            default: {
                state_ = AtlasReaderState::TakeReading;
                break;
            }
            }
        }
        else {
            state_ = AtlasReaderState::TakeReading;
        }
        break;
    }
    case AtlasReaderState::TakeReading: {
        sendCommand("R", ATLAS_DEFAULT_DELAY_COMMAND_READ);
        state_ = AtlasReaderState::WaitingOnReply;
        postReplyState_ = AtlasReaderState::ParseReading;
        break;
    }
    case AtlasReaderState::ParseReading: {
        if (sleepAfter_) {
            state_ = AtlasReaderState::Sleep;
        }
        else {
            state_ = AtlasReaderState::Idle;
        }
        break;
    }
    case AtlasReaderState::WaitingOnEmptyReply: {
        switch (readReply(nullptr, 0 )) {
        case AtlasResponseCode::NotReady: {
            nextCheckAt_ = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
            break;
        }
        case AtlasResponseCode::Error: {
            if (commandAttempts_ == 3) {
                state_ = postReplyState_;
            }
            else {
                nextCheckAt_ = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
                state_ = retryState_;
                commandAttempts_++;
            }
            break;
        }
        default: {
            if (postReplyState_ == AtlasReaderState::ConfigureParameter) {
                parameter_++;
            }
            state_ = postReplyState_;
            break;
        }
        }
        break;
    }
    case AtlasReaderState::WaitingOnReply: {
        switch (readReply(buffer_, sizeof(buffer_))) {
        case AtlasResponseCode::NotReady: {
            nextCheckAt_ = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
            break;
        }
        case AtlasResponseCode::Error: {
            if (commandAttempts_ == 3) {
                state_ = postReplyState_;
            }
            else {
                nextCheckAt_ = millis() + ATLAS_DEFAULT_DELAY_NOT_READY;
                state_ = retryState_;
                commandAttempts_++;
            }
            break;
        }
        default: {
            state_ = postReplyState_;
            break;
        }
        }
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

AtlasResponseCode EzoAtlas::singleCommand(const char *command) {
    postReplyState_ = AtlasReaderState::Sleep;
    state_ = AtlasReaderState::WaitingOnReply;
    return sendCommand(command, 300);
}

AtlasResponseCode EzoAtlas::sendCommand(const char *str, uint32_t readDelay) {
    bus_->send(address_, str);

    if (retryState_ != state_) {
        retryState_ = state_;
        commandAttempts_ = 0;
    }
    nextCheckAt_  = millis() + readDelay;

    loginfof(Log, "Atlas(0x%x, %s) <- ('%s', %lu) (ca=%d)", address_, typeName(), str, readDelay, commandAttempts_);

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

const char *EzoAtlas::typeName() {
    switch (type_) {
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

static bool shouldRetry(AtlasResponseCode code, char *buffer) {
    if (code == AtlasResponseCode::NotReady) {
        return true;
    }
    if (code == AtlasResponseCode::NoData) {
        if (buffer != nullptr) {
            return true;
        }
    }
    return false;
}

AtlasResponseCode EzoAtlas::readReply(char *buffer, size_t length) {
    bus_->requestFrom(address_, 1 + length, (uint8_t)1);

    auto code = static_cast<AtlasResponseCode>(bus_->read());
    if (code == AtlasResponseCode::Error) {
        loginfof(Log, "Atlas(0x%x, %s) -> (code=0x%x) (%d/%d)", address_, typeName(), code,
                 tries_, atlas_configuration.maximum_atlas_retries);
        return code;
    }
    if (shouldRetry(code, buffer)) {
        tries_++;
        loginfof(Log, "Atlas(0x%x, %s) -> (code=0x%x) (%d/%d)", address_, typeName(), code,
                 tries_, atlas_configuration.maximum_atlas_retries);
        if (tries_ == atlas_configuration.maximum_atlas_retries) {
            tries_ = 0;
            return AtlasResponseCode::Error;
        }
        return AtlasResponseCode::NotReady;
    }

    tries_ = 0;

    size_t i = 0;
    while (bus_->available()) {
        auto c = bus_->read();
        if (buffer != nullptr && i < length - 1) {
            buffer[i++] = c;
        }
        if (c == 0) {
            bus_->endTransmission();
            break;
        }
    }

    if (buffer != nullptr) {
        buffer[i] = 0;
        loginfof(Log, "Atlas(0x%x, %s) -> ('%s') (code=0x%x)", address_, typeName(), buffer, code);

        if (type_ == AtlasSensorType::Unknown) {
            type_ = getSensorType(buffer);
        }

        if (postReplyState_ == AtlasReaderState::ParseReading) {
            String line = buffer;
            int16_t position = 0;

            numberOfValues_ = 0;

            while (position < (int16_t)line.length()) {
                int16_t index = line.indexOf(',', position);
                if (index < 0) {
                    index = line.length();
                }
                if (numberOfValues_ < ATLAS_MAXIMUM_NUMBER_OF_VALUES)  {
                    if (index > position) {
                        String part = line.substring(position, index);
                        values_[numberOfValues_++] = part.toFloat();
                        position = index + 1;
                    }
                }
                else {
                    loginfof(Log, "Error: Too many values");
                    break;
                }
            }

            if (numberOfValues_ == 0) {
                loginfof(Log, "No values, retry?");
            }
        }
    }
    else {
        loginfof(Log, "Atlas(0x%x, %s) -> (code=0x%x)", address_, typeName(), code);
    }

    return code;
}

size_t EzoAtlas::readAll(float *values) {
    for (size_t i = 0; i < numberOfValues_; ++i) {
        *values++ = this->values_[i];
    }
    size_t number = numberOfValues_;
    numberOfValues_ = 0;
    return number;
}

}
