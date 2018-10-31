#ifndef FK_SENSOR_POWER_H_INCLUDED
#define FK_SENSOR_POWER_H_INCLUDED

#include <fk-module.h>

namespace fk {

constexpr uint32_t AtlasPowerOnTime = 1 * Seconds;
constexpr uint32_t AtlasPowerOffTime = 5 * Seconds;

class SensorPower : public Task {
private:
    uint32_t last_powered_on_{ 0 };
    uint32_t turn_off_at_{ 0 };

public:
    SensorPower();

public:
    void enable();
    bool ready();
    void busy();

public:
    TaskEval task() override;

private:
    bool enabled() const;
    void enabled(bool enabled);

};

}

#endif
