#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>

#include "sensors.h"

const uint8_t ATLAS_RESPONSE_CODE_NO_DATA = 0xff;
const uint8_t ATLAS_RESPONSE_CODE_NOT_READY = 0xfe;
const uint8_t ATLAS_RESPONSE_CODE_ERROR = 0x2;
const uint8_t ATLAS_RESPONSE_CODE_SUCCESS = 0x1;

enum class AtlasReaderState {
    Start,
    Blink,
    LedsOn,
    Sleep,
    Sleeping,
    Idle,
    Reading,
    WaitingOnReply,
    WaitingOnStatus,
    Done
};

class AtlasReader : public SensorReader {
private:
    AtlasReaderState state { AtlasReaderState::Start };
    AtlasReaderState postReplyState { AtlasReaderState::Idle };
    uint32_t nextReadAt { 0 };
    uint8_t readings { 0 };
    TwoWire *bus { nullptr };
    uint8_t address { 0 };

public:
    AtlasReader(TwoWire *theBus, uint8_t theAddress);
    bool setup() override;
    bool tick() override;
    bool beginReading() override;
    bool hasReading() override;

private:
    void info();
    void ledsOff();
    void find();
    void ledsOn();
    void sleep();
    void read();

public:
    uint8_t sendCommand(const char *str, uint32_t readDelay = 300);
    uint8_t readReply(char *buffer, size_t length);

};

#endif
