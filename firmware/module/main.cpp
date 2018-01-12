/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller? Maybe with log levels?
 * TODO: Calibration mode. With the help of the app, this is a big deal I think.
 */
#include <Arduino.h>
#include <Wire.h>

#include <fk-module.h>

#include "atlas.h"

namespace fk {

using WireAddress = uint8_t;
using PinNumber = uint8_t;

const WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

class AtlasModule : public Module {
private:
    SensorModule *atlasSensors;
    TwoWireBus bus{ Wire11and13 };

public:
    AtlasModule(ModuleInfo &info, SensorModule &atlasSensors);

public:
    ModuleReadingStatus beginReading(PendingSensorReading &pending) override;
    ModuleReadingStatus readingStatus(PendingSensorReading &pending) override;

};

AtlasModule::AtlasModule(ModuleInfo &info, SensorModule &atlasSensors) : Module(bus, info), atlasSensors(&atlasSensors) {
}

ModuleReadingStatus AtlasModule::beginReading(PendingSensorReading &pending) {
    atlasSensors->beginReading();

    return ModuleReadingStatus{ 1000 };
}

ModuleReadingStatus AtlasModule::readingStatus(PendingSensorReading &pending) {
    debugfpln("Atlas", "Number: %d", atlasSensors->numberOfReadingsReady());
    if (atlasSensors->numberOfReadingsReady() == 8) {
        // Order: Ec1,2,3,4,Temp,pH,Do,ORP
        float values[atlasSensors->numberOfReadingsReady()];
        size_t size = atlasSensors->readAll(values);

        auto readings = pending.readings;
        readings[ 0].value = values[0];
        readings[ 1].value = values[1];
        readings[ 2].value = values[2];
        readings[ 3].value = values[3];
        readings[ 4].value = values[4];
        readings[ 5].value = values[5];
        readings[ 6].value = values[6];
        readings[ 7].value = values[7];
        pending.elapsed -= millis();
        for (auto i = 0; i < 8; ++i) {
            readings[i].status = SensorReadingStatus::Done;
            readings[i].time = clock.getTime();
        }
    }

    return ModuleReadingStatus{};
}

}

extern "C" {

void setup() {
    Serial.begin(115200);

    while (!Serial && millis() < 2000) {
        delay(100);
    }

    debugfpln("Module", "Starting (%lu free)", fk_free_memory());

    fk::ModuleInfo info = {
        8,
        8,
        "Atlas",
        {
            {"Ec", "µS/cm",},
            {"TDS", "°ppm",},
            {"Salinity", "",},
            {"SG", "",},
            {"Temp", "C",},
            {"pH", "",},
            {"DO", "mg/L",},
            {"ORP", "mV",},
        },
        {
            {}, {}, {}, {}, {}, {}, {}, {},
        },
    };

    fk::TwoWireBus sensorBus{ Wire };

    fk::AtlasReader ec(sensorBus, fk::ATLAS_SENSOR_EC_DEFAULT_ADDRESS);
    fk::AtlasReader temp(sensorBus, fk::ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS);
    fk::AtlasReader ph(sensorBus, fk::ATLAS_SENSOR_PH_DEFAULT_ADDRESS);
    fk::AtlasReader dissolvedOxygen(sensorBus, fk::ATLAS_SENSOR_DO_DEFAULT_ADDRESS);
    fk::AtlasReader orp(sensorBus, fk::ATLAS_SENSOR_ORP_DEFAULT_ADDRESS);

    fk::Sensor *sensors[] = { &ec, &ph, &dissolvedOxygen, &orp, &temp };
    fk::SensorModule atlasSensors(sensors);
    fk::AtlasModule module(info, atlasSensors);

    uint32_t idleStart = 0;

    module.begin();

    // TODO: Investigate. I would see hangs if I used a slower speed.
    sensorBus.begin(400000);

    while (true) {
        atlasSensors.tick();
        module.tick();

        if (false) {
            if (atlasSensors.isIdle()) {
                if (idleStart == 0 ) {
                    idleStart = millis() + 20000;
                    module.resume();
                    sensorBus.flush();
                }
                else {
                    module.tick();
                }

                if (idleStart < millis()) {
                    sensorBus.begin(400000);
                    atlasSensors.beginReading();
                    idleStart = 0;
                }
            }
        }

        delay(10);
    }
}

void loop() {
}

}
