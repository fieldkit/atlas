#include "sensors.h"
#include "atlas_hardware.h"

namespace fk {

EnableSensors::EnableSensors() : Task("EnableSensors") {
}

void EnableSensors::enqueued() {
    if (!enabled()) {
        log("Powering up");

        enabled(true);

        expire_at_ = fk_uptime() + 2000;
        last_powered_on_ = fk_uptime();
    }
    else {
        expire_at_ = fk_uptime();
    }
}

TaskEval EnableSensors::task() {
    if (expire_at_ > 0) {
        if (fk_uptime() > expire_at_) {
            expire_at_ = 0;
            return TaskEval::done();
        }
    }

    if (enabled()) {
        auto elapsed = fk_uptime() - last_powered_on_;
        if (elapsed > 10 * 1000) {
            log("Powering down");
            enabled(false);
            last_powered_on_ = 0;
        }
    }

    return TaskEval::idle();
}

bool EnableSensors::enabled() const {
    return digitalRead(FK_ATLAS_PIN_PERIPH_ENABLE);
}

void EnableSensors::enabled(bool enabled) {
    digitalWrite(FK_ATLAS_PIN_PERIPH_ENABLE, enabled ? HIGH : LOW);
}

bool SensorModule::setup() {
    pinMode(FK_ATLAS_PIN_PERIPH_ENABLE, OUTPUT);

    // TODO: Investigate. I would see hangs if I used a slower speed.
    sensorBus.begin(400000);

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

void SensorModule::compensate(Compensation compensation) {
    for (size_t i = 0; i < numberOfSensors; ++i) {
        sensors[i]->compensate(compensation);
    }
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

AtlasReader *SensorModule::getSensorByType(fk_atlas_SensorType type) {
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
