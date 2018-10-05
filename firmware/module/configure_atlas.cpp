#include "configure_atlas.h"

namespace fk {

void ConfigureAtlas::task() {
    auto sensorPower = atlasServices().sensorPower;
    sensorPower->enable(AtlasDefaultMinimum);
    while (simple_task_run(*sensorPower)) {
        services().alive();
    }

    auto atlasSensors = atlasServices().atlasSensors;
    atlasSensors->enqueued();
    while (simple_task_run(*atlasSensors)) {
        services().alive();
    }

    transit<ModuleIdle>();
}

}
