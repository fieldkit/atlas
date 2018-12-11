#ifndef FK_ATLAS_MODULE_H_INCLUDED
#define FK_ATLAS_MODULE_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>

#include "atlas_hardware.h"
#include "atlas.h"

#include "configure_atlas.h"
#include "custom_atlas_query.h"
#include "take_atlas_readings.h"

namespace fk {

class AtlasModule : public Module<MinimumFlashState> {
private:
    TwoWireBus moduleBus{ Wire4and3 };
    SensorPower sensorPower{ *moduleServices().hardware };
    SensorModule atlasSensors{ sensorPower };
    AttachedSensors attachedSensors{ moduleServices().readings };
    AtlasServices atlasServices{
        &sensorPower,
        &atlasSensors,
        &attachedSensors,
    };

public:
    AtlasModule(ModuleInfo &info);

public:
    void begin() override;
    ModuleHooks *hooks() override {
        return &atlasServices;
    }

public:
    ModuleStates states() override {
        return {
            fk::ModuleFsm::deferred<ConfigureAtlas>(),
            fk::ModuleFsm::deferred<TakeAtlasReadings>(),
            fk::ModuleFsm::deferred<CustomAtlasQuery>()
        };
    }

};

}

#endif
