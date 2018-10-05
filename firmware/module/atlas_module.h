#ifndef FK_ATLAS_MODULE_H_INCLUDED
#define FK_ATLAS_MODULE_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>

#include "atlas_hardware.h"
#include "atlas.h"

#include "custom_atlas_query.h"
#include "take_atlas_readings.h"

namespace fk {

class AtlasModule : public Module<MinimumFlashState> {
private:
    #ifdef FK_MODULE_WIRE11AND13
    TwoWireBus moduleBus{ Wire11and13 };
    #else
    TwoWireBus moduleBus{ Wire4and3 };
    #endif

    SensorPower sensorPower;
    SensorModule atlasSensors;
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
    void tick() override;

public:
    fk::ModuleStates states() override {
        return {
            fk::ModuleFsm::deferred<fk::ConfigureModule>(),
            fk::ModuleFsm::deferred<TakeAtlasReadings>(),
            fk::ModuleFsm::deferred<CustomAtlasQuery>()
        };
    }

};

}

#endif
