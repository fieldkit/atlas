#include <Arduino.h>
#include <Wire.h>

#include <fk-module.h>

#include "atlas_module.h"
#include "board_definition.h"

extern "C" {

void setup() {
    Serial.begin(115200);

    fk::board.disable_everything();

    while (!Serial && millis() < 2000) {
        delay(100);
    }

    if (!Serial) {
        // The call to end here seems to free up some memory.
        Serial.end();
        Serial5.begin(115200);
        log_uart_set(Serial5);
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

    fk::SensorInfo sensors[fk::NumberOfReadings] = {
        {"ec", "µS/cm",},
        {"tds", "°ppm",},
        {"salinity", "",},
        #if defined(FK_ENABLE_ATLAS_EZO)
        {"sg", "",},
        #endif
        {"ph", "",},
        {"do", "mg/L",},
        #if defined(FK_ENABLE_ATLAS_ORP)
        {"orp", "mV",},
        #endif
        {"temp", "C",},
        #if defined(FK_ENABLE_MS5803)
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
        readings,
        0,
        fk_module_RequiredUptime_READINGS_ONLY,
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
