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

size_t Sensor::numberOfReadingsReady() const {
    return reader.numberOfReadingsReady();
}

bool Sensor::isIdle() const {
    return reader.isIdle();
}

size_t Sensor::readAll(float *values) {
    return reader.readAll(values);
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

size_t SensorModule::numberOfReadingsReady() const {
    size_t total = 0;
    for (size_t i = 0; i < numberOfSensors; ++i) {
        size_t number = sensors[i].numberOfReadingsReady();
        if (number == 0) {
            return 0;
        }
        total += number;
    }
    return total;
}

size_t SensorModule::readAll(float *values) {
    size_t total = 0;
    for (size_t i = 0; i < numberOfSensors; ++i) {
        size_t number = sensors[i].readAll(values);
        values += number;
        total += number;
    }
    return total;
}
