#ifndef FK_SENSOR_POWER_H_INCLUDED
#define FK_SENSOR_POWER_H_INCLUDED

#include <fk-module.h>

namespace fk {

class SensorPower : public Task {
private:
    uint32_t last_powered_on_{ 0 };
    uint32_t expire_at_{ 0 };

public:
    SensorPower();

public:
    void enqueued() override;
    TaskEval task() override;

private:
    bool enabled() const;
    void enabled(bool enabled);

};

}

#endif
