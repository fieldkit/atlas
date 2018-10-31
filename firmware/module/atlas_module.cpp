#include "atlas_module.h"
#include "module_idle.h"

namespace fk {

AtlasModule::AtlasModule(ModuleInfo &info) :
    Module(moduleBus, info, { FK_ATLAS_PIN_FLASH, FK_ATLAS_PIN_PERIPH_ENABLE, FK_ATLAS_PERIPH_MINIMUM_ENABLE_TIME }) {
}

void AtlasModule::begin() {
    Module::begin();

    atlasSensors.setup();

    attachedSensors.setup();

    AtlasModuleState::atlasServices(atlasServices);
}

void AtlasModule::tick() {
    Module::tick();
    sensorPower.task();
}

}
