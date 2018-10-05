#include "atlas_module.h"
#include "module_idle.h"

namespace fk {

AttachedSensors::AttachedSensors(PendingReadings *readings) : readings_(readings) {
}

bool AttachedSensors::setup() {
    #ifdef FK_ENABLE_MS5803
    ms5803Pressure_.reset();
    ms5803Pressure_.begin();
    #endif
    return true;
}

bool AttachedSensors::take(size_t number) {
    #ifdef FK_ENABLE_MS5803
    auto pressureTemperature = ms5803Pressure_.getTemperature(CELSIUS, ADC_512);
    auto pressureAbsolute = ms5803Pressure_.getPressure(ADC_4096);

    readings_->done(number++, pressureTemperature);

    readings_->done(number++, pressureAbsolute);

    sdebug() << "Ms5803: " << pressureAbsolute << " " << pressureTemperature << " " << number << endl;
    #endif

    return true;
}

AtlasServices *AtlasModuleState::atlasServices_{ nullptr };

void TakeAtlasReadings::task() {
    auto atlasSensors = atlasServices().atlasSensors;
    auto enableSensors = atlasServices().enableSensors;

    atlasSensors->compensate(atlasServices().compensation);

    atlasSensors->beginReading(services().readings->remaining() <= 1);

    enableSensors->enabled();

    enableSensors->enqueued();

    while (simple_task_run(*enableSensors)) {
        services().alive();
    }

    atlasServices().atlasSensors->enqueued();

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

AtlasModule::AtlasModule(ModuleInfo &info, TwoWireBus &sensorBus) :
    Module(moduleBus, info, { FK_ATLAS_PIN_FLASH, FK_ATLAS_PIN_PERIPH_ENABLE }),
    sensorBus(&sensorBus), sensors {
        &ec,
        &ph,
        &dissolvedOxygen,
        #ifdef FK_ENABLE_ATLAS_ORP
        &orp,
        #endif
        &temp
    }, atlasSensors(sensors) {
}

void AtlasModule::begin() {
    Module::begin();

    atlasSensors.setup();

    AtlasModuleState::atlasServices(atlasServices);

    attachedSensors.setup();
}

void AtlasModule::tick() {
    Module::tick();

    /*
    if (elapsedSinceActivity() > 5000) {
        if (enableSensors.enabled()) {
            log("Disabling peripherals.");
            enableSensors.enabled(false);
        }
    }
    */
}

AtlasReader &AtlasModule::getSensor(fk_atlas_SensorType type) {
    switch (type) {
    case fk_atlas_SensorType_PH: return ph;
    case fk_atlas_SensorType_TEMP: return temp;
    #ifdef FK_ENABLE_ATLAS_ORP
    case fk_atlas_SensorType_ORP: return orp;
    #endif
    case fk_atlas_SensorType_DO: return dissolvedOxygen;
    case fk_atlas_SensorType_EC: return ec;
    }

    fk_assert(false);
}

}
