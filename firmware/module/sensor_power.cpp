#include "sensor_power.h"
#include "atlas_hardware.h"

namespace fk {

SensorPower::SensorPower() : Task("SensorPower") {
}

void SensorPower::enable(uint32_t minimum) {
    minimum_ = minimum;

    if (!enabled()) {
        log("Powering up (%lums)", minimum_);

        enabled(true);

        expire_at_ = fk_uptime() + AtlasPowerOnTime;
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
        if (elapsed > minimum_) {
            log("Powering down");
            enabled(false);
            last_powered_on_ = 0;
            minimum_ = 0;
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
