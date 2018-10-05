#ifndef FK_SENSOR_POWER_H_INCLUDED
#define FK_SENSOR_POWER_H_INCLUDED

#include <fk-module.h>

namespace fk {

constexpr uint32_t AtlasDefaultMinimum = 20 * Seconds;
constexpr uint32_t AtlasCustomQueryMinimum = 5 * Minutes;
constexpr uint32_t AtlasPowerOnTime = 2 * Seconds;

class SensorPower : public Task {
private:
    uint32_t last_powered_on_{ 0 };
    uint32_t expire_at_{ 0 };
    uint32_t minimum_{ 0 };

public:
    SensorPower();

public:
    void enable(uint32_t minimum);

public:
    TaskEval task() override;

private:
    bool enabled() const;
    void enabled(bool enabled);

};

}

#endif
