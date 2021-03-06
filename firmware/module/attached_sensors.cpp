#include "attached_sensors.h"

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

}
