#include "atlas_module.h"
#include "module_idle.h"
#include "board_definition.h"

namespace fk {

AtlasModule::AtlasModule(ModuleInfo &info) :
    Module(moduleBus, info, { FK_ATLAS_PIN_FLASH_CS, FK_ATLAS_PIN_PERIPH_ENABLE, FK_ATLAS_PERIPH_MINIMUM_ENABLE_TIME }) {
}

void AtlasModule::begin() {
    Module::begin();

    atlasSensors.setup();

    attachedSensors.setup();

    AtlasModuleState::atlasServices(atlasServices);
}

}
