#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

#include <Arduino.h>

class SensorReader {
public:
    virtual bool setup() = 0;
    virtual bool tick() = 0;
    virtual bool beginReading() = 0;
    virtual bool hasReading() = 0;
    virtual bool isIdle() = 0;
};

class Sensor {
private:
    SensorReader &reader;
    bool available;

public:
    Sensor(SensorReader &reader);

    bool setup();
    bool tick();
    bool isAvailable();
    bool isIdle();
    void beginTakeReading();
    bool hasReadingReady();
};

class SensorModule {
private:
    Sensor *sensors { nullptr };
    const size_t numberOfSensors { 0 };

public:
    template<size_t N>
    SensorModule(Sensor (&sensors)[N]) : sensors(sensors), numberOfSensors(N) {
    }

    bool setup();
    bool tick();
    bool isBusy();
    bool isIdle();
    void beginTakeReading();
    bool hasReadingReady();

};

#endif
