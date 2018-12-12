#include "sensor_power.h"
#include "board_definition.h"

namespace fk {

SensorPower::SensorPower(ModuleHardware &hardware) : Task("SensorPower"), hardware_(&hardware) {
}

void SensorPower::enable() {
    if (atlas_power_.take()) {
        log("Atlas on");
        board.enable_atlas_modules();
    }

    atlas_power_.touch();
}

bool SensorPower::ready() {
    if (!atlas_power_.on()) {
        enable();
    }
    return atlas_power_.has_been_on_for(AtlasPowerOnTime);
}

void SensorPower::busy() {
    atlas_power_.touch();
}

TaskEval SensorPower::task() {
    if (atlas_power_.release()) {
        log("Atlas off");
        board.disable_atlas_modules();
    }

    return TaskEval::idle();
}

}
