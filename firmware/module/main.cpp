/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller? Maybe with log levels?
 * TODO: Calibration mode. With the help of the app, this is a big deal I think.
 */
#include <Arduino.h>
#include <Wire.h>

#include <fk-module.h>

#include "atlas_module.h"

namespace fk {

using WireAddress = uint8_t;
using PinNumber = uint8_t;

const WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

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
