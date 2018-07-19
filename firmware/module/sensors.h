#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

#include <Arduino.h>
#undef min
#undef max
#include <functional>
#include <active_object.h>

namespace fk {

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
    bool waitingOnSiblings { false };
    std::function<void()> onFree;

    TickSlice() {
    }

    TickSlice(std::function<void()> f) : waitingOnSiblings(true), onFree(f) {
    }

    void free() {
        onFree();
    }
};

class Sensor {
public:
    virtual bool setup() = 0;
    virtual TickSlice tick() = 0;
    virtual void compensate(Compensation compensation) = 0;
    virtual bool beginReading(bool sleep) = 0;
    virtual size_t numberOfReadingsReady() const = 0;
    virtual bool isIdle() const = 0;
    virtual size_t readAll(float *values) = 0;
};

class EnableSensors : public Task {
private:
    uint32_t expireAt_{ 0 };

public:
    EnableSensors();

public:
    void enqueued() override;
    TaskEval task() override;

public:
    bool enabled() const;
    void enabled(bool enabled);

};

class SensorModule : public Task {
private:
    Sensor **sensors { nullptr };
    const size_t numberOfSensors { 0 };

public:
    template<size_t N>
    SensorModule(Sensor *(&sensors)[N]) : Task("Sensors"), sensors(sensors), numberOfSensors(N) {
    }

public:
    bool setup();
    TaskEval task() override;

    void compensate(Compensation compensation);
    void beginReading(bool sleep);
    size_t readAll(float *values);

    bool isBusy() const;
    bool isIdle() const;
    size_t numberOfReadingsReady() const;

    Sensor *getSensor(size_t index) {
        return sensors[index];
    }

};

}

#endif
