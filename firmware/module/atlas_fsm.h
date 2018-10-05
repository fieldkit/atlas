#ifndef FK_ATLAS_FSM_H_INCLUDED
#define FK_ATLAS_FSM_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>

#include "atlas_hardware.h"
#include "atlas.h"
#include "sensors.h"
#include "sensor_power.h"
#include "attached_sensors.h"

namespace fk {

struct AtlasServices {
    SensorPower *sensorPower;
    SensorModule *atlasSensors;
    AttachedSensors *attachedSensors;
    Compensation compensation;

    AtlasServices(SensorPower *sensorPower, SensorModule *atlasSensors, AttachedSensors *attachedSensors)
        : sensorPower(sensorPower), atlasSensors(atlasSensors),  attachedSensors(attachedSensors) {
    }
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
