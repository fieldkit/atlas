#ifndef FK_ATLAS_FSM_H_INCLUDED
#define FK_ATLAS_FSM_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>

#include "atlas_hardware.h"
#include "atlas.h"
#include "sensors.h"

#ifdef FK_ENABLE_MS5803
#include <SparkFun_MS5803_I2C.h>
#endif

namespace fk {

class AttachedSensors {
private:
    PendingReadings *readings_;
    #ifdef FK_ENABLE_MS5803
    MS5803 ms5803Pressure_{ ADDRESS_HIGH };
    #endif

public:
    AttachedSensors(PendingReadings *readings);

public:
    bool setup();

public:
    bool take(size_t number);

};

struct AtlasServices {
    EnableSensors *enableSensors;
    SensorModule *atlasSensors;
    AttachedSensors *attachedSensors;
    Compensation compensation;

    AtlasServices(EnableSensors *enableSensors, SensorModule *atlasSensors, AttachedSensors *attachedSensors)
        : enableSensors(enableSensors), atlasSensors(atlasSensors),  attachedSensors(attachedSensors) {
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
