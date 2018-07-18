#include "atlas-hardware.h"
#include "atlas.h"

namespace fk {

EnableSensors::EnableSensors() : Task("EnableSensors") {
}

void EnableSensors::enqueued() {
    if (!digitalRead(FK_ATLAS_PIN_PERIPH_ENABLE)) {
        log("Enabling Sensors");

        digitalWrite(FK_ATLAS_PIN_PERIPH_ENABLE, HIGH);

        expireAt_ = fk_uptime() + 2000;
    }
    else {
        expireAt_ = fk_uptime();
    }
}

TaskEval EnableSensors::task() {
    if (fk_uptime() > expireAt_) {
        log("Done");
        return TaskEval::done();
    }
    return TaskEval::idle();
}

bool SensorModule::setup() {
    pinMode(FK_ATLAS_PIN_PERIPH_ENABLE, OUTPUT);

    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i]->setup();
    }

    return true;
}

TaskEval SensorModule::task() {
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
        return TaskEval::busy();
    }
    return TaskEval::done();
}

bool SensorModule::isBusy() const {
    return !isIdle();
}

bool SensorModule::isIdle() const {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        if (!sensors[i]->isIdle()) {
            return false;
        }
    }
    return true;
}

void SensorModule::beginReading(bool sleep) {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i]->beginReading(sleep);
    }
}

size_t SensorModule::numberOfReadingsReady() const {
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

size_t SensorModule::readAll(float *values) {
    size_t total = 0;
    for (size_t i = 0; i < numberOfSensors; ++i) {
        size_t number = sensors[i]->readAll(values);
        values += number;
        total += number;
    }

    return total;
}

}
