#include "take_atlas_readings.h"

namespace fk {

void TakeAtlasReadings::task() {
    auto sensorPower = atlasServices().sensorPower;
    sensorPower->enable(AtlasDefaultMinimum);
    while (simple_task_run(*sensorPower)) {
        services().alive();
    }

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
        // Order: Ec1,2,3,4,pH,Do,ORP,Temp
        float values[atlasSensors->numberOfReadingsReady()];
        auto size = atlasSensors->readAll(values);
        auto now = clock.getTime();

        for (auto i = 0; i < size; ++i) {
            services().readings->done(i, values[i]);
        }

        // Temperature is always the final reading.
        atlasServices().compensation = Compensation{ values[size - 1] };

        atlasServices().attachedSensors->take(size);
    }

    transit<ModuleIdle>();
}

}
