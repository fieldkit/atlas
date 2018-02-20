#ifndef FK_ATLAS_MODULE_H_INCLUDED
#define FK_ATLAS_MODULE_H_INCLUDED

#include <fk-module.h>
#include <fk-atlas-protocol.h>
#include "atlas.h"

namespace fk {

using WireAddress = uint8_t;
using PinNumber = uint8_t;

const WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

class AtlasModule : public Module {
private:
    StaticPool<128> pool{ "AtlasModule" };
    TwoWireBus *sensorBus;
    TwoWireBus moduleBus{ Wire11and13 };
    AtlasReader ec{*sensorBus, ATLAS_SENSOR_EC_DEFAULT_ADDRESS};
    AtlasReader ph{*sensorBus, ATLAS_SENSOR_PH_DEFAULT_ADDRESS};
    AtlasReader dissolvedOxygen{*sensorBus, ATLAS_SENSOR_DO_DEFAULT_ADDRESS};
    AtlasReader orp{*sensorBus, ATLAS_SENSOR_ORP_DEFAULT_ADDRESS};
    AtlasReader temp{*sensorBus, ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS};
    Sensor *sensors[5];
    SensorModule atlasSensors;

public:
    AtlasModule(ModuleInfo &info, TwoWireBus &sensorBus);

public:
    void begin() override;
    ModuleReadingStatus beginReading(PendingSensorReading &pending) override;
    ModuleReadingStatus readingStatus(PendingSensorReading &pending) override;
    TaskEval message(ModuleQueryMessage &query, ModuleReplyMessage &reply) override;

private:
    AtlasReader &getSensor(fk_atlas_SensorType type);

};

}

#endif
