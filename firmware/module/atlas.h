#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#include "two_wire.h"
#include "sensors.h"

namespace fk {

enum class AtlasResponseCode : uint8_t {
    NoData = 0xff,
    NotReady = 0xfe,
    Error = 0x2,
    Success = 0x1,
};

const uint32_t ATLAS_DEFAULT_DELAY_COMMAND = 300;
const uint32_t ATLAS_DEFAULT_DELAY_COMMAND_READ = 1000;
const uint32_t ATLAS_DEFAULT_DELAY_NOT_READY = 100;

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

class AtlasReader : public Sensor {
private:
    TwoWireBus *bus;
    uint8_t address { 0 };
    AtlasSensorType type { AtlasSensorType::Unknown };
    AtlasReaderState state { AtlasReaderState::Start };
    AtlasReaderState postReplyState { AtlasReaderState::Idle };
    uint32_t nextCheckAt { 0 };
    float values[ATLAS_MAXIMUM_NUMBER_OF_VALUES];
    size_t numberOfValues { 0 };
    char buffer[ATLAS_MAXIMUM_COMMAND_LENGTH];

public:
    AtlasReader(TwoWireBus &bus, uint8_t theAddress);
    bool setup() override;
    TickSlice tick() override;
    bool beginReading() override;
    size_t readAll(float *values) override;
    size_t numberOfReadingsReady() const override;
    bool isIdle() const override;
    void sleep();
    AtlasResponseCode singleCommand(const char *command);
    const char *lastReply() {
        return buffer;
    }

private:
    AtlasResponseCode sendCommand(const char *str, uint32_t readDelay = ATLAS_DEFAULT_DELAY_COMMAND);
    AtlasResponseCode readReply(char *buffer, size_t length);

};

}

#endif
