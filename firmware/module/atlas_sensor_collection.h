#ifndef FK_SENSOR_COLLECTION_H_INCLUDED
#define FK_SENSOR_COLLECTION_H_INCLUDED

#include <task.h>
#include <fk-atlas-protocol.h>

#include "sensor_power.h"

#include "ezo_atlas.h"
#include "oem_atlas.h"

namespace fk {

constexpr size_t NumberOfSensors = 4
    #ifdef FK_ENABLE_ATLAS_ORP
    + 1
    #endif
    ;

using AtlasSensor = OemAtlas;

class AtlasSensorCollection : public Task {
private:
    SensorPower *sensorPower;

    using WireAddress = uint8_t;

    TwoWireBus sensorBus{ Wire };
    AtlasSensor temp{ sensorBus, AtlasSensor::TEMP_DEFAULT_ADDRESS };
    AtlasSensor ec{ sensorBus, AtlasSensor::EC_DEFAULT_ADDRESS };
    AtlasSensor ph{ sensorBus, AtlasSensor::PH_DEFAULT_ADDRESS };
    AtlasSensor dissolvedOxygen{ sensorBus, AtlasSensor::DO_DEFAULT_ADDRESS };
    #ifdef FK_ENABLE_ATLAS_ORP
    AtlasSensor orp{ sensorBus, AtlasSensor::ORP_DEFAULT_ADDRESS };
    #endif
    AtlasSensor *sensors[NumberOfSensors];
    const size_t numberOfSensors { NumberOfSensors };

public:
    AtlasSensorCollection(SensorPower &sensorPower) : Task("ASC"), sensorPower(&sensorPower),
        sensors {
            &temp,
            &ec,
            &ph,
            &dissolvedOxygen,
            #ifdef FK_ENABLE_ATLAS_ORP
            &orp,
            #endif
            } {
    }


public:
    bool setup();
    TaskEval task() override;

public:
    bool isBusy() const;
    bool isIdle() const;
    void compensate(Compensation compensation);
    void beginReading(bool sleep);
    size_t readAll(float *values);
    size_t numberOfReadingsReady() const;

public:
    AtlasSensor *getSensor(size_t index) {
        return sensors[index];
    }

    AtlasSensor *getSensorByType(fk_atlas_SensorType type);

};

}

#endif
