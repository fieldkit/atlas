#include "sensor_power.h"
#include "board_definition.h"

namespace fk {

SensorPower::SensorPower(ModuleHardware &hardware) : Task("SensorPower"), hardware_(&hardware) {
}

void SensorPower::enable() {
    if (last_powered_on_ == 0) {
        // digitalWrite(FK_ATLAS_PIN_ATLAS_ENABLE, HIGH);
        // TwoWire bus1{ Wire };
        // bus1.begin();
        // hardware_->flash_take();
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
    if (last_powered_on_ > 0) {
        if (fk_uptime() > turn_off_at_) {
            // hardware_->flash_release();
            // TwoWire bus1{ Wire };
            // bus1.end();
            // digitalWrite(FK_ATLAS_PIN_ATLAS_ENABLE, LOW);
            last_powered_on_ = 0;
            turn_off_at_ = 0;
        }
    }

    return TaskEval::idle();
}

}
