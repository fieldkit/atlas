#include "atlas_module.h"
#include "module_idle.h"

namespace fk {

AtlasModule::AtlasModule(ModuleInfo &info) :
    Module(moduleBus, info, { FK_ATLAS_PIN_FLASH, FK_ATLAS_PIN_PERIPH_ENABLE }) {
}

void AtlasModule::begin() {
    Module::begin();

    atlasSensors.setup();

    attachedSensors.setup();

    AtlasModuleState::atlasServices(atlasServices);

    // auto atlasSensors = atlasServices().atlasSensors;
    // auto enableSensors = atlasServices().enableSensors;
    enableSensors.enqueued();
    while (simple_task_run(enableSensors)) {
    }
    atlasSensors.enqueued();
    while (simple_task_run(atlasSensors)) {
    }
}

void AtlasModule::tick() {
    Module::tick();
    enableSensors.task();
}

}
