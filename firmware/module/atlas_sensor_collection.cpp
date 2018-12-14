#include "atlas_sensor_collection.h"

namespace fk {

bool AtlasSensorCollection::setup() {
    // TODO: Investigate. I would see hangs if I used a slower speed.
    // sensorBus.begin(400000);

    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i]->setup();
    }

    return true;
}

TaskEval AtlasSensorCollection::task() {
    if (!sensorPower->ready()) {
        return TaskEval::busy();
    }

    auto allWaiting = true;
    TickSlice slices[numberOfSensors];
    for (size_t i = 0; i < numberOfSensors; ++i) {
        slices[i] = sensors[i]->tick();
        if (!slices[i].waitingOnSiblings) {
            allWaiting = false;
        }
    }

    if (allWaiting) {
        for (size_t i = 0; i < numberOfSensors; ++i) {
            slices[i].free();
        }
    }

    if (isBusy()) {
        sensorPower->busy();
        return TaskEval::busy();
    }
    return TaskEval::done();
}

bool AtlasSensorCollection::isBusy() const {
    return !isIdle();
}

bool AtlasSensorCollection::isIdle() const {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i]->isIdle()) {
            return false;
        }
    }
    return true;
}

void AtlasSensorCollection::compensate(Compensation compensation) {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i]->compensate(compensation);
    }
}

void AtlasSensorCollection::beginReading(bool sleep) {
    sensorPower->enable();

    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i]->beginReading(sleep);
    }
}

size_t AtlasSensorCollection::numberOfReadingsReady() const {
    size_t total = 0;
    for (size_t i = 0; i < numberOfSensors; ++i) {
        size_t number = sensors[i]->numberOfReadingsReady();
        if (number == 0) {
            return 0;
        }
        total += number;
    }
    return total;
}

size_t AtlasSensorCollection::readAll(float *values) {
    size_t total = 0;
    for (size_t i = 0; i < numberOfSensors; ++i) {
        size_t number = sensors[i]->readAll(values);
        values += number;
        total += number;
    }

    return total;
}

AtlasSensor *AtlasSensorCollection::getSensorByType(fk_atlas_SensorType type) {
    switch (type) {
    case fk_atlas_SensorType_PH: return &ph;
    case fk_atlas_SensorType_TEMP: return &temp;
    #ifdef FK_ENABLE_ATLAS_ORP
    case fk_atlas_SensorType_ORP: return &orp;
    #endif
    case fk_atlas_SensorType_DO: return &dissolvedOxygen;
    case fk_atlas_SensorType_EC: return &ec;
    }

    fk_assert(false);
    return nullptr;
}

}
