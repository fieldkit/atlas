#include "atlas_fsm.h"

namespace fk {

AtlasServices *AtlasModuleState::atlasServices_{ nullptr };

void AtlasServices::alive() {
    sensorPower->task();
    atlasSensors->task();
}

}
