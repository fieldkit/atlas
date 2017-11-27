#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

#include <Arduino.h>

class Sensor {
public:
    virtual bool setup() = 0;
    virtual bool tick() = 0;
    virtual bool beginReading() = 0;
    virtual size_t numberOfReadingsReady() const = 0;
    virtual bool isIdle() const = 0;
    virtual size_t readAll(float *values) = 0;
};

class SensorModule {
private:
    Sensor **sensors { nullptr };
    const size_t numberOfSensors { 0 };

public:
    template<size_t N>
    SensorModule(Sensor *(&sensors)[N]) : sensors(sensors), numberOfSensors(N) {
    }

    bool setup();
    bool tick();
    void beginReading();
    size_t readAll(float *values);

    bool isBusy() const;
    bool isIdle() const;
    size_t numberOfReadingsReady() const;
};

#endif
