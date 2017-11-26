#include "sensors.h"

Sensor::Sensor(SensorReader &reader) : reader(reader) {
}

bool Sensor::isAvailable() {
    return available;
}

bool Sensor::setup() {
    reader.setup();
    available = true;
    return true;
}

bool Sensor::tick() {
    reader.tick();
    return true;
}

void Sensor::beginTakeReading() {
    reader.beginReading();
}

bool Sensor::hasReadingReady() {
    return false;
}

bool Sensor::isIdle() {
    return reader.isIdle();
}

bool SensorModule::setup() {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i].setup();
    }
    return true;
}

bool SensorModule::tick() {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i].tick();
    }
    return true;
}

bool SensorModule::isBusy() {
    return !isIdle();
}

bool SensorModule::isIdle() {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i].isIdle()) {
            return false;
        }
    }
    return true;
}

void SensorModule::beginTakeReading() {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i].beginTakeReading();
    }
}

bool SensorModule::hasReadingReady() {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i].hasReadingReady()) {
            return false;
        }
    }
    return true;
}
