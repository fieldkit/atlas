#ifndef FK_EZO_ATLAS_H_INCLUDED
#define FK_EZO_ATLAS_H_INCLUDED

#include "atlas_common.h"

namespace fk {

class EzoAtlas {
private:
    static constexpr uint32_t ATLAS_DEFAULT_DELAY_COMMAND = 300;
    static constexpr uint32_t ATLAS_DEFAULT_DELAY_COMMAND_READ = 1000;
    static constexpr uint32_t ATLAS_DEFAULT_DELAY_NOT_READY = 300;

    static constexpr size_t ATLAS_MAXIMUM_COMMAND_LENGTH = 20;
    static constexpr size_t ATLAS_MAXIMUM_NUMBER_OF_VALUES = 4;

    enum class State {
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

    enum class ResponseCode : uint8_t {
        NoData = 0xff,
        NotReady = 0xfe,
        Error = 0x2,
        Success = 0x1,
    };

private:
    TwoWireBus *bus_;
    uint8_t address_{ 0 };
    AtlasSensorType type_{ AtlasSensorType::Unknown };
    State state_{ State::Start };
    State postReplyState_{ State::Idle };
    State retryState_{ State::Idle };
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
    bool beginReading(bool sleep);
    size_t readAll(float *values);
    size_t numberOfReadingsReady() const;
    bool isIdle() const;
    bool singleCommand(const char *command);

public:
    void compensate(Compensation c) {
        compensation_ = c;
    }
    const char *lastReply() {
        return buffer_;
    }

private:
    ResponseCode sendCommand(const char *str, uint32_t readDelay = ATLAS_DEFAULT_DELAY_COMMAND);
    ResponseCode readReply(char *buffer, size_t length);
    bool shouldRetry(ResponseCode code, char *buffer);
    void sleep();
    const char *typeName();

};

}

#endif
