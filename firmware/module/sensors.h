#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

#include <Arduino.h>

class SensorReader {
public:
    virtual bool setup() = 0;
    virtual bool tick() = 0;
    virtual bool beginReading() = 0;
    virtual bool hasReading() const = 0;
    virtual bool isIdle() const = 0;
};

class Sensor {
private:
    SensorReader &reader;
    bool available;

public:
    Sensor(SensorReader &reader);

    bool setup();
    bool tick();
    void beginReading();

    bool isAvailable() const;
    bool isIdle() const;
    bool hasReadingReady() const;
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
    void beginReading();

    bool isBusy() const;
    bool isIdle() const;
    bool hasReadingReady() const;

};

#endif
