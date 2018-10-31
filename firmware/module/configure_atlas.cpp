#include "configure_atlas.h"

namespace fk {

void ConfigureAtlas::task() {
    auto sensorPower = atlasServices().sensorPower;
    sensorPower->enable();

    auto atlasSensors = atlasServices().atlasSensors;
    atlasSensors->enqueued();

    transit<ModuleIdle>();
}

}
