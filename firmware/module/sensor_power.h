#ifndef FK_SENSOR_POWER_H_INCLUDED
#define FK_SENSOR_POWER_H_INCLUDED

#include <fk-module.h>

namespace fk {

class SensorPower : public Task {
private:
    static constexpr uint32_t AtlasPowerOnTime = 1 * Seconds;
    static constexpr uint32_t AtlasPowerMinimumOnTime = 5 * Seconds;

private:
    ModuleHardware *hardware_;
    PowerSwitch atlas_power_{ AtlasPowerMinimumOnTime };

public:
    SensorPower(ModuleHardware &hardware);

public:
    void enable();
    bool ready();
    void busy();

public:
    TaskEval task() override;

};

}

#endif
