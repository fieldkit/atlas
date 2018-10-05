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

    firmware_version_set(FIRMWARE_GIT_HASH);
    firmware_build_set(FIRMWARE_BUILD);
    firmware_compiled_set(DateTime(__DATE__, __TIME__).unixtime());

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
    #ifdef FK_MODULE_WIRE11AND13
    loginfof("Module", "FK_MODULE_WIRE11AND13");
    #endif

    fk::SensorInfo sensors[fk::NumberOfReadings] = {
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
    };

    fk::SensorReading readings[fk::NumberOfReadings];

    fk::ModuleInfo info = {
        fk_module_ModuleType_SENSOR,
        8,
        fk::NumberOfReadings,
        10,
        "Atlas",
        "fk-atlas",
        sensors,
        readings
    };

    fk::AtlasModule module(info);

    module.begin();

    while (true) {
        module.tick();
        delay(10);
    }
}

void loop() {
}

}
