#include "sensor_power.h"
#include "atlas_hardware.h"

namespace fk {

SensorPower::SensorPower() : Task("SensorPower") {
}

void SensorPower::enable() {
    if (!enabled()) {
        log("Powering up...");

        enabled(true);

        last_powered_on_ = fk_uptime();
    }

    busy();
}

bool SensorPower::ready() {
    if (last_powered_on_ == 0) {
        return false;
    }

    return fk_uptime() - last_powered_on_ > AtlasPowerOnTime;
}

void SensorPower::busy() {
    turn_off_at_ = fk_uptime() + AtlasPowerOffTime;
}

TaskEval SensorPower::task() {
    if (enabled()) {
        if (fk_uptime() > turn_off_at_) {
            log("Powering down");
            enabled(false);
            last_powered_on_ = 0;
            turn_off_at_ = 0;
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
