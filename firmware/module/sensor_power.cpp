#include "sensor_power.h"
#include "atlas_hardware.h"

namespace fk {

SensorPower::SensorPower(ModuleHardware &hardware) : Task("SensorPower"), hardware_(&hardware) {
}

void SensorPower::enable() {
    if (last_powered_on_ == 0) {
        hardware_->flash_take();
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
            hardware_->flash_release();
            last_powered_on_ = 0;
            turn_off_at_ = 0;
        }
    }

    return TaskEval::idle();
}

}
