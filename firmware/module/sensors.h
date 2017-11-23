#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

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
    Sensor(SensorReader &reader) : reader(reader) {
    }

    bool isAvailable() {
        return available;
    }

    bool hasReadingReady() {
        return readingReady;
    }

    bool setup() {
        reader.setup();
        available = true;
        return true;
    }

    bool tick() {
        reader.tick();
        return true;
    }

    void beginTakeReading() {
        reader.beginReading();
    }
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

    bool setup() {
        for (auto i = 0; i < numberOfSensors; ++i) {
            sensors[i].setup();
        }
        return true;
    }

    bool tick() {
        for (auto i = 0; i < numberOfSensors; ++i) {
            sensors[i].tick();
        }
        return true;
    }

    bool isBusy() {
        return state == SensorModuleState::Busy;
    }

    void beginTakeReading() {
        for (auto i = 0; i < numberOfSensors; ++i) {
            sensors[i].beginTakeReading();
        }
        state = SensorModuleState::Busy;
    }

    bool hasReadingReady() {
        for (auto i = 0; i < numberOfSensors; ++i) {
            if (!sensors[i].hasReadingReady()) {
                return false;
            }
        }
        return true;
    }
};

#endif
