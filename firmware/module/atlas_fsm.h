#ifndef FK_ATLAS_FSM_H_INCLUDED
#define FK_ATLAS_FSM_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>

#include "sensor_power.h"
#include "atlas_sensor_collection.h"
#include "attached_sensors.h"

namespace fk {

struct AtlasServices : public ModuleHooks {
    SensorPower *sensorPower;
    AtlasSensorCollection *atlasSensors;
    AttachedSensors *attachedSensors;
    Compensation compensation;

    AtlasServices(SensorPower *sensorPower, AtlasSensorCollection *atlasSensors, AttachedSensors *attachedSensors)
        : sensorPower(sensorPower), atlasSensors(atlasSensors),  attachedSensors(attachedSensors) {
    }

    void alive() override;
};

class AtlasModuleState : public ModuleServicesState {
private:
    static AtlasServices *atlasServices_;

public:
    static AtlasServices &atlasServices() {
        fk_assert(atlasServices_ != nullptr);
        return *atlasServices_;
    }

    static void atlasServices(AtlasServices &newServices) {
        atlasServices_ = &newServices;
    }

};

}

#endif
