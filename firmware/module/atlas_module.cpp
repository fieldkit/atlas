#include "atlas_module.h"
#include "module_idle.h"

namespace fk {

AtlasModule::AtlasModule(ModuleInfo &info) :
    Module(moduleBus, info) {
}

void AtlasModule::begin() {
    Module::begin();

    atlasSensors.setup();

    attachedSensors.setup();

    AtlasModuleState::atlasServices(atlasServices);
}

}
