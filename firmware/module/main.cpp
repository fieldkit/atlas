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

extern "C" {

void setup() {
    Serial.begin(115200);

    pinMode(fk::FK_ATLAS_PIN_PERIPH_ENABLE, OUTPUT);
    digitalWrite(fk::FK_ATLAS_PIN_PERIPH_ENABLE, LOW);

    while (!Serial && millis() < 2000) {
        delay(100);
    }

    loginfof("Module", "Starting (%lu free)", fk_free_memory());

    #ifdef FK_ENABLE_MS5803
    loginfof("Module", "FK_ENABLE_MS5803");
    #endif
    #ifdef FK_ENABLE_ATLAS_ORP
    loginfof("Module", "FK_ENABLE_ATLAS_ORP");
    #endif
    #ifdef FK_DISABLE_ATLAS_ORP
    loginfof("Module", "FK_DISABLE_ATLAS_ORP");
    #endif

    fk::ModuleInfo info = {
        fk_module_ModuleType_SENSOR,
        8,
        fk::NumberOfReadings,
        "Atlas",
        {
            {"ec", "µS/cm",},
            {"tds", "°ppm",},
            {"salinity", "",},
            {"sg", "",},
            {"ph", "",},
            {"do", "mg/L",},
            #ifdef FK_ENABLE_ATLAS_ORP
            {"orp", "mV",},
            #endif
            {"temp", "C",},
            #ifdef FK_ENABLE_MS5803
            {"temp_bottom", "C",},
            {"pressure", "mbar",},
            #endif
        },
        {
            {}, {}, {}, {}, {}, {}, {}, {},
        },
    };

    fk::TwoWireBus sensorBus{ Wire };
    fk::AtlasModule module(info, sensorBus);

    module.begin();

    // TODO: Investigate. I would see hangs if I used a slower speed.
    sensorBus.begin(400000);

    while (true) {
        module.tick();
        delay(10);
    }
}

void loop() {
}

}
