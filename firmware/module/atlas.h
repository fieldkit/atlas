#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#include "sensors.h"

const uint8_t ATLAS_RESPONSE_CODE_NO_DATA = 0xff;
const uint8_t ATLAS_RESPONSE_CODE_NOT_READY = 0xfe;
const uint8_t ATLAS_RESPONSE_CODE_ERROR = 0x2;
const uint8_t ATLAS_RESPONSE_CODE_SUCCESS = 0x1;

const uint32_t ATLAS_DEFAULT_DELAY_COMMAND = 300;
const uint32_t ATLAS_DEFAULT_DELAY_NOT_READY = 100;

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
    bool hasReading() override;
    bool isIdle() override;

private:
    void info();
    void ledsOff();
    void find();
    void ledsOn();
    void sleep();
    void read();

    uint8_t sendCommand(const char *str, uint32_t readDelay = ATLAS_DEFAULT_DELAY_COMMAND);
    uint8_t readReply(char *buffer, size_t length);

};

#endif
