#ifndef FK_SENSOR_COLLECTION_H_INCLUDED
#define FK_SENSOR_COLLECTION_H_INCLUDED

#include <task.h>
#include <fk-atlas-protocol.h>

#include "sensor_power.h"

#include "ezo_atlas.h"

namespace fk {

constexpr size_t NumberOfSensors = 4
    #ifdef FK_ENABLE_ATLAS_ORP
    + 1
    #endif
    ;

class AtlasSensorCollection : public Task {
private:
    SensorPower *sensorPower;

    using WireAddress = uint8_t;

    static constexpr WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
    static constexpr WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
    static constexpr WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
    static constexpr WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
    static constexpr WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

    TwoWireBus sensorBus{ Wire };
    EzoAtlas ec{sensorBus, ATLAS_SENSOR_EC_DEFAULT_ADDRESS};
    EzoAtlas ph{sensorBus, ATLAS_SENSOR_PH_DEFAULT_ADDRESS};
    EzoAtlas dissolvedOxygen{sensorBus, ATLAS_SENSOR_DO_DEFAULT_ADDRESS};
    #ifdef FK_ENABLE_ATLAS_ORP
    EzoAtlas orp{sensorBus, ATLAS_SENSOR_ORP_DEFAULT_ADDRESS};
    #endif
    EzoAtlas temp{sensorBus, ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS};
    EzoAtlas *sensors[NumberOfSensors];
    const size_t numberOfSensors { NumberOfSensors };

public:
    AtlasSensorCollection(SensorPower &sensorPower) :
        Task("ASC"),
        sensorPower(&sensorPower),
        sensors {
            &ec,
            &ph,
            &dissolvedOxygen,
            #ifdef FK_ENABLE_ATLAS_ORP
            &orp,
            #endif
            &temp
            } {
    }


public:
    bool setup();
    TaskEval task() override;

    void compensate(Compensation compensation);
    void beginReading(bool sleep);
    size_t readAll(float *values);

    bool isBusy() const;
    bool isIdle() const;
    size_t numberOfReadingsReady() const;

    EzoAtlas *getSensor(size_t index) {
        return sensors[index];
    }

    EzoAtlas *getSensorByType(fk_atlas_SensorType type);

};

}

#endif
