#include "take_atlas_readings.h"
#include "reboot_device.h"

namespace fk {

void TakeAtlasReadings::task() {
    auto sensorPower = atlasServices().sensorPower;
    sensorPower->enable();

    auto atlasSensors = atlasServices().atlasSensors;
    atlasSensors->compensate(atlasServices().compensation);
    atlasSensors->beginReading(services().readings->remaining() <= 1);
    atlasSensors->enqueued();
    while (simple_task_run(*atlasSensors)) {
        services().alive();
    }

    static_assert(NumberOfReadings >= NumberOfAtlasReadings, "NumberOfReadings should be >= NumberOfAtlasReadings");

    log("NumberOfReadingsReady: %d", atlasSensors->numberOfReadingsReady());

    if (atlasSensors->numberOfReadingsReady() == NumberOfAtlasReadings) {
        float values[atlasSensors->numberOfReadingsReady()];
        auto size = atlasSensors->readAll(values);
        auto now = clock.getTime();

        for (auto i = 0; i < size; ++i) {
            services().readings->done(i, values[i]);
        }

        // Temperature is always the final reading.
        atlasServices().compensation = Compensation{ values[size - 1] };

        atlasServices().attachedSensors->take(size);

        transit<ModuleIdle>();
    }
    else {
        // We're rebooting here because we're not seeing all the values we're
        // expecting. Rebooting will trigger a re-initialization. Sometimes this
        // is because of the EC sensor output parameters or some other strange
        // hiccup.
        transit<RebootDevice>();
    }
}

}
