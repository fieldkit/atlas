#include "sensors.h"

Sensor::Sensor(SensorReader &reader) : reader(reader) {
}

bool Sensor::isAvailable() const {
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

void Sensor::beginReading() {
    reader.beginReading();
}

bool Sensor::hasReadingReady() const {
    return false;
}

bool Sensor::isIdle() const {
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

bool SensorModule::isBusy() const {
    return !isIdle();
}

bool SensorModule::isIdle() const {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i].isIdle()) {
            return false;
        }
    }
    return true;
}

void SensorModule::beginReading() {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i].beginReading();
    }
}

bool SensorModule::hasReadingReady() const {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i].hasReadingReady()) {
            return false;
        }
    }
    return true;
}
