#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

#include <Arduino.h>

class SensorReader {
public:
    virtual bool setup() = 0;
    virtual bool tick() = 0;
    virtual bool beginReading() = 0;
    virtual bool hasReading() = 0;
};

class Sensor {
private:
    SensorReader &reader;
    bool available;
    bool readingReady;

public:
    Sensor(SensorReader &reader);

    bool setup();
    bool tick();
    bool isAvailable();
    void beginTakeReading();
    bool hasReadingReady();
};

enum class SensorModuleState {
    Idle,
    Busy
};

class SensorModule {
private:
    Sensor *sensors { nullptr };
    const size_t numberOfSensors { 0 };
    SensorModuleState state { SensorModuleState::Idle };

public:
    template<size_t N>
    SensorModule(Sensor (&sensors)[N]) : sensors(sensors), numberOfSensors(N) {
    }

    bool setup();
    bool tick();
    bool isBusy();
    void beginTakeReading();
    bool hasReadingReady();

};

#endif
