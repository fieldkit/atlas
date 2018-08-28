#ifndef FK_ATLAS_MODULE_H_INCLUDED
#define FK_ATLAS_MODULE_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>
#include "atlas_hardware.h"
#include "atlas.h"

#ifdef FK_ENABLE_MS5803
#include <SparkFun_MS5803_I2C.h>
#endif

namespace fk {

using WireAddress = uint8_t;
using PinNumber = uint8_t;

constexpr WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
constexpr WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
constexpr WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
constexpr WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
constexpr WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

constexpr size_t NumberOfSensors = 4
    #ifdef FK_ENABLE_ATLAS_ORP
    + 1
    #endif
    ;

constexpr size_t NumberOfAtlasReadings = 7
    #ifdef FK_ENABLE_ATLAS_ORP
    + 1
    #endif
    ;

constexpr size_t NumberOfReadings = NumberOfAtlasReadings
    #ifdef FK_ENABLE_MS5803
    + 2
    #endif
    ;

struct AtlasServices {
    EnableSensors *enableSensors;
    SensorModule *atlasSensors;
    Compensation compensation;

    AtlasServices(EnableSensors *enableSensors, SensorModule *atlasSensors) : enableSensors(enableSensors), atlasSensors(atlasSensors) {
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

class TakeAtlasReadings : public AtlasModuleState {
public:
    const char *name() const override {
        return "TakeAtlasReadings";
    }

public:
    void task() override;
};

class CustomAtlasQuery : public AtlasModuleState {
public:
    const char *name() const override {
        return "CustomAtlasQuery";
    }

public:
    void task() override;
};

class AtlasModule : public Module {
private:
    StaticPool<128> pool{ "AtlasModule" };
    TwoWireBus *sensorBus;
    #ifdef FK_MODULE_WIRE11AND13
    TwoWireBus moduleBus{ Wire11and13 };
    #else
    TwoWireBus moduleBus{ Wire4and3 };
    #endif
    AtlasReader ec{*sensorBus, ATLAS_SENSOR_EC_DEFAULT_ADDRESS};
    AtlasReader ph{*sensorBus, ATLAS_SENSOR_PH_DEFAULT_ADDRESS};
    AtlasReader dissolvedOxygen{*sensorBus, ATLAS_SENSOR_DO_DEFAULT_ADDRESS};
    #ifdef FK_ENABLE_ATLAS_ORP
    AtlasReader orp{*sensorBus, ATLAS_SENSOR_ORP_DEFAULT_ADDRESS};
    #endif
    AtlasReader temp{*sensorBus, ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS};
    Sensor *sensors[NumberOfSensors];
    EnableSensors enableSensors;
    SensorModule atlasSensors;
    AtlasServices atlasServices{
        &enableSensors,
        &atlasSensors
    };
    #ifdef FK_ENABLE_MS5803
    MS5803 ms5803Pressure{ ADDRESS_HIGH };
    #endif

public:
    AtlasModule(ModuleInfo &info, TwoWireBus &sensorBus);

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

private:
    AtlasReader &getSensor(fk_atlas_SensorType type);

};

}

#endif
