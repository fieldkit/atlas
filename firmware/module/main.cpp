/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller? Maybe with log levels?
 * TODO: Calibration mode. With the help of the app, this is a big deal I think.
 */
#include <Arduino.h>
#include <Wire.h>

#include "leds.h"
#include "atlas.h"
#include "fk-module.h"
#include "debug.h"

void moduleMain();

using WireAddress = uint8_t;
using PinNumber = uint8_t;

const PinNumber LED_PIN = 13;

const WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

AtlasReader ec(&Wire, ATLAS_SENSOR_EC_DEFAULT_ADDRESS);
AtlasReader temp(&Wire, ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS);
AtlasReader ph(&Wire, ATLAS_SENSOR_PH_DEFAULT_ADDRESS);
AtlasReader dissolvedOxygen(&Wire, ATLAS_SENSOR_DO_DEFAULT_ADDRESS);
AtlasReader orp(&Wire, ATLAS_SENSOR_ORP_DEFAULT_ADDRESS);

Sensor *sensors[] = { &ec, &ph, &dissolvedOxygen, &orp, &temp };
SensorModule sensorModule(sensors);
FkLeds leds;

uint8_t yield_reading(fk_module_t *fkm, fk_pool_t *fkp) {
    fk_module_readings_t *readings = (fk_module_readings_t *)fk_pool_malloc(fkp, sizeof(fk_module_readings_t));
    APR_RING_INIT(readings, fk_module_reading_t, link);

    for (size_t i = 0; i < 3; ++i) {
        fk_module_reading_t *reading = (fk_module_reading_t *)fk_pool_malloc(fkp, sizeof(fk_module_reading_t));
        reading->sensor = i;
        reading->time = fkm->rtc.getTime();
        reading->value = random(20, 150);
        APR_RING_INSERT_TAIL(readings, reading, fk_module_reading_t, link);
    }

    fk_module_done_reading(fkm, readings);

    return true;
}

void setup() {
    leds.setup();

    Serial.begin(115200);

    while (!Serial) {
        delay(250);
    }

    sensorModule.setup();

    fk_pool_t *scan_pool = nullptr;
    fk_pool_create(&scan_pool, 512, nullptr);

    fk_module_sensor_metadata_t sensors[] = {
        {
            .id = 0,
            .name = "Ec",
            .unitOfMeasure = "µS/cm",
        },
        {
            .id = 1,
            .name = "TDS",
            .unitOfMeasure = "°ppm",
        },
        {
            .id = 2,
            .name = "Salinity",
            .unitOfMeasure = "",
        },
        {
            .id = 3,
            .name = "SG",
            .unitOfMeasure = "",
        },
        {
            .id = 4,
            .name = "Temp",
            .unitOfMeasure = "C",
        },
        {
            .id = 5,
            .name = "pH",
            .unitOfMeasure = "",
        },
        {
            .id = 6,
            .name = "DO",
            .unitOfMeasure = "mg/L",
        },
        {
            .id = 7,
            .name = "ORP",
            .unitOfMeasure = "mV",
        },
    };

    fk_module_t module = {
        .address = 8,
        .name = "Atlas",
        .number_of_sensors = sizeof(sensors) / sizeof(fk_module_sensor_metadata_t),
        .sensors = sensors,
        .begin_reading = yield_reading,
        .state = fk_module_state_t::START,
        .reply_pool = nullptr,
        .readings_pool = nullptr,
        .readings = nullptr,
        .pending = nullptr
    };

    if (!fk_module_start(&module, nullptr)) {
        debugfln("error creating module");
        return;
    }

    Wire.begin();
    // TODO: Investigate. I would see hangs if I used a slower speed.
    Wire.setClock(400000);

    debugfln("Ready (%d)", fk_free_memory());

    uint32_t idleStart = 0;

    while (true) {
        sensorModule.tick();

        if (sensorModule.numberOfReadingsReady() > 0) {
            // Order: Ec1,2,3,4,Temp,pH,Do,ORP
            float values[sensorModule.numberOfReadingsReady()];
            size_t size = sensorModule.readAll(values);

            debugf("Reading: ");
            for (size_t i = 0; i < size; ++i) {
                debugf("%f ", values[i]);
            }
            debugfln("");

            debugfln("Status (%d)", fk_free_memory());
        }

        if (sensorModule.isIdle()) {
            if (idleStart == 0 ) {
                idleStart = millis() + 20000;

                fk_module_resume(&module);

            }
            else {
                fk_module_tick(&module);
            }

            if (idleStart < millis()) {
                Wire.begin();
                // TODO: Investigate. I would see hangs if I used a slower speed.
                Wire.setClock(400000);

                sensorModule.beginReading();
                idleStart = 0;
            }
        }

        delay(10);
    }
}

void loop() {
    delay(10);
}
