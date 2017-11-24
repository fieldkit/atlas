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
    return readingReady;
}

bool SensorModule::setup() {
    for (auto i = 0; i < numberOfSensors; ++i) {
        sensors[i].setup();
    }
    return true;
}

bool SensorModule::tick() {
    for (auto i = 0; i < numberOfSensors; ++i) {
        sensors[i].tick();
    }
    return true;
}

bool SensorModule::isBusy() {
    return state == SensorModuleState::Busy;
}

void SensorModule::beginTakeReading() {
    for (auto i = 0; i < numberOfSensors; ++i) {
        sensors[i].beginTakeReading();
    }
    state = SensorModuleState::Busy;
}

bool SensorModule::hasReadingReady() {
    for (auto i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i].hasReadingReady()) {
            return false;
        }
    }
    return true;
}
