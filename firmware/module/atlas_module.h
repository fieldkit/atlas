#ifndef FK_ATLAS_MODULE_H_INCLUDED
#define FK_ATLAS_MODULE_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>
#include "atlas-hardware.h"
#include "atlas.h"

namespace fk {

using WireAddress = uint8_t;
using PinNumber = uint8_t;

constexpr WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
constexpr WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
constexpr WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
constexpr WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
constexpr WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

#ifdef FK_ENABLE_ATLAS_ORP
constexpr size_t NumberOfSensors = 5;
constexpr size_t NumberOfReadings = 8;
#else
constexpr size_t NumberOfSensors = 4;
constexpr size_t NumberOfReadings = 7;
#endif

class AtlasModule : public Module {
private:
    StaticPool<128> pool{ "AtlasModule" };
    TwoWireBus *sensorBus;
    TwoWireBus moduleBus{ Wire4and3 };
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

public:
    AtlasModule(ModuleInfo &info, TwoWireBus &sensorBus);

public:
    void begin() override;
    void tick() override;
    ModuleReadingStatus beginReading(PendingSensorReading &pending) override;
    ModuleReadingStatus readingStatus(PendingSensorReading &pending) override;
    TaskEval message(ModuleQueryMessage &query, ModuleReplyMessage &reply) override;

private:
    AtlasReader &getSensor(fk_atlas_SensorType type);

};

}

#endif
