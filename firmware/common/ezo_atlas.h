#ifndef FK_EZO_ATLAS_H_INCLUDED
#define FK_EZO_ATLAS_H_INCLUDED

#include "atlas_common.h"

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
    bool waitingOnSiblings{ false };
    std::function<void()> onFree;

    TickSlice() {
    }

    TickSlice(std::function<void()> f) : waitingOnSiblings(true), onFree(f) {
    }

    void free() {
        onFree();
    }
};

class EzoAtlas {
private:
    TwoWireBus *bus_;
    uint8_t address_{ 0 };
    AtlasSensorType type_{ AtlasSensorType::Unknown };
    AtlasReaderState state_{ AtlasReaderState::Start };
    AtlasReaderState postReplyState_{ AtlasReaderState::Idle };
    AtlasReaderState retryState_{ AtlasReaderState::Idle };
    uint32_t nextCheckAt_{ 0 };
    float values_[ATLAS_MAXIMUM_NUMBER_OF_VALUES];
    size_t numberOfValues_{ 0 };
    char buffer_[ATLAS_MAXIMUM_COMMAND_LENGTH];
    uint8_t commandAttempts_{ 0 };
    uint8_t tries_{ 0 };
    bool sleepAfter_{ true };
    Compensation compensation_;
    uint8_t parameter_{ 0 };

public:
    EzoAtlas(TwoWireBus &bus, uint8_t theAddress);

public:
    bool setup();
    TickSlice tick();
    void compensate(Compensation c) {
        compensation_ = c;
    }
    bool beginReading(bool sleep);
    size_t readAll(float *values);
    size_t numberOfReadingsReady() const;
    bool isIdle() const;
    void sleep();
    AtlasResponseCode singleCommand(const char *command);
    const char *lastReply() {
        return buffer_;
    }

private:
    AtlasResponseCode sendCommand(const char *str, uint32_t readDelay = ATLAS_DEFAULT_DELAY_COMMAND);
    AtlasResponseCode readReply(char *buffer, size_t length);
    const char *typeName();

};

}

#endif
