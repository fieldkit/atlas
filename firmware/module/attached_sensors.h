#ifndef FK_ATTACHED_SENSORS_H_INCLUDED
#define FK_ATTACHED_SENSORS_H_INCLUDED

#include <fk-module.h>

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

}

#endif
