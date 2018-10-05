#include "sensor_power.h"
#include "atlas_hardware.h"

namespace fk {

SensorPower::SensorPower() : Task("SensorPower") {
}

void SensorPower::enqueued() {
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

TaskEval SensorPower::task() {
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

bool SensorPower::enabled() const {
    return digitalRead(FK_ATLAS_PIN_PERIPH_ENABLE);
}

void SensorPower::enabled(bool enabled) {
    digitalWrite(FK_ATLAS_PIN_PERIPH_ENABLE, enabled ? HIGH : LOW);
}

}
