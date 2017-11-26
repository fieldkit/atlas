#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#include "sensors.h"

enum class AtlasResponseCode : uint8_t {
    NoData = 0xff,
    NotReady = 0xfe,
    Error = 0x2,
    Success = 0x1,
};

const uint32_t ATLAS_DEFAULT_DELAY_COMMAND = 300;
const uint32_t ATLAS_DEFAULT_DELAY_NOT_READY = 100;
const uint32_t ATLAS_DEFAULT_DELAY_SLEEP = 1000;

enum class AtlasReaderState {
    Start,
    Status,
    Blink,
    LedsOn,
    Sleep,
    Sleeping,
    Idle,
    Reading,
    WaitingOnReply,
    WaitingOnEmptyReply
};

class AtlasReader : public SensorReader {
private:
    TwoWire *bus { nullptr };
    uint8_t address { 0 };
    AtlasReaderState state { AtlasReaderState::Start };
    AtlasReaderState postReplyState { AtlasReaderState::Idle };
    uint32_t nextCheckAt { 0 };
    uint8_t readings { 0 };

public:
    AtlasReader(TwoWire *theBus, uint8_t theAddress);
    bool setup() override;
    bool tick() override;
    bool beginReading() override;
    bool hasReading() const override;
    bool isIdle() const override;

private:
    void info();
    void ledsOff();
    void find();
    void ledsOn();
    void sleep();
    void read();

    AtlasResponseCode sendCommand(const char *str, uint32_t readDelay = ATLAS_DEFAULT_DELAY_COMMAND);
    AtlasResponseCode readReply(char *buffer, size_t length);

};

#endif
