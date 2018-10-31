#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#undef min
#undef max
#include <functional>
#include "two_wire.h"

namespace fk {

enum class AtlasResponseCode : uint8_t {
    NoData = 0xff,
    NotReady = 0xfe,
    Error = 0x2,
    Success = 0x1,
};

const uint32_t ATLAS_DEFAULT_DELAY_COMMAND = 300;
const uint32_t ATLAS_DEFAULT_DELAY_COMMAND_READ = 1000;
const uint32_t ATLAS_DEFAULT_DELAY_NOT_READY = 300;

const size_t ATLAS_MAXIMUM_COMMAND_LENGTH = 20;
const size_t ATLAS_MAXIMUM_NUMBER_OF_VALUES = 4;

enum class AtlasReaderState {
    Start,
    Status,
    Blink,
    LedsOn,
    WantSleep,
    Sleep,
    Sleeping,
    Idle,
    ParseReading,
    QueryParameters,
    ConfigureParameter,
    QueryProbeType,
    ConfigureProbeType,
    QueryProtocolLock,
    LockProtocol,
    ApplyCompensation,
    TakeReading,
    WaitingOnReply,
    WaitingOnEmptyReply,
    SendCommand,
};

enum class AtlasSensorType {
    Unknown,
    Ph,
    Ec,
    Orp,
    Do,
    Temp
};

struct Compensation {
    float temperature;
    bool valid;

    Compensation() {
    }

    Compensation(float temperature) : temperature(temperature), valid(true) {
    }

    operator bool() {
        return valid && temperature >= -1000.0f;
    }
};

struct TickSlice {
    bool waitingOnSiblings { false };
    std::function<void()> onFree;

    TickSlice() {
    }

    TickSlice(std::function<void()> f) : waitingOnSiblings(true), onFree(f) {
    }

    void free() {
        onFree();
    }
};

class AtlasReader {
private:
    TwoWireBus *bus;
    uint8_t address { 0 };
    AtlasSensorType type { AtlasSensorType::Unknown };
    AtlasReaderState state { AtlasReaderState::Start };
    AtlasReaderState postReplyState { AtlasReaderState::Idle };
    AtlasReaderState retryState { AtlasReaderState::Idle };
    uint32_t nextCheckAt { 0 };
    float values[ATLAS_MAXIMUM_NUMBER_OF_VALUES];
    size_t numberOfValues { 0 };
    char buffer[ATLAS_MAXIMUM_COMMAND_LENGTH];
    uint8_t commandAttempts{ 0 };
    uint8_t tries{ 0 };
    bool sleepAfter{ true };
    Compensation compensation;
    uint8_t parameter{ 0 };

public:
    AtlasReader(TwoWireBus &bus, uint8_t theAddress);
    bool setup();
    TickSlice tick();
    void compensate(Compensation c) {
        compensation = c;
    }
    bool beginReading(bool sleep);
    size_t readAll(float *values);
    size_t numberOfReadingsReady() const;
    bool isIdle() const;
    void sleep();
    AtlasResponseCode singleCommand(const char *command);
    const char *lastReply() {
        return buffer;
    }

private:
    AtlasResponseCode sendCommand(const char *str, uint32_t readDelay = ATLAS_DEFAULT_DELAY_COMMAND);
    AtlasResponseCode readReply(char *buffer, size_t length);
    const char *typeName();

};

}

#endif
