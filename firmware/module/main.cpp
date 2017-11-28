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

FkLeds leds;

SensorModule module(sensors);

void setup() {
    leds.setup();

    leds.on();

    Serial.begin(115200);

    Wire.begin();

    // TODO: Investigate. I would see hangs if I used a slower speed.
    Wire.setClock(400000);

    while (!Serial) {
        delay(250);
    }

    module.setup();

    // Never happens, just wanna link the module code in.
    if (module.numberOfReadingsReady() == 100) {
        moduleMain();
    }
}

void loop() {
    module.tick();

    if (module.numberOfReadingsReady() > 0) {
        // Order: Ec1,2,3,4,Temp,pH,Do,ORP
        float values[module.numberOfReadingsReady()];
        size_t size = module.readAll(values);
        for (size_t i = 0; i < size; ++i) {
            debugf("%f ", values[i]);
        }
        debugfln("");
    }
    if (module.isIdle()) {
        delay(10000);
        module.beginReading();
    }

    delay(10);
}

uint8_t dummy_reading(fk_module_t *fkm, fk_pool_t *fkp) {
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

void moduleMain() {
    digitalWrite(LED_PIN, LOW);

    debugfln("dummy: ready, checking (free = %d)...", fk_free_memory());

    fk_pool_t *scan_pool = nullptr;
    fk_pool_create(&scan_pool, 512, nullptr);

    debugfln("dummy: acting as slave");

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
        .begin_reading = dummy_reading,
        .state = fk_module_state_t::START,
        .reply_pool = nullptr,
        .readings_pool = nullptr,
        .readings = nullptr,
        .pending = nullptr
    };

    if (!fk_module_start(&module, nullptr)) {
        debugfln("dummy: error creating module");
        return;
    }

    while (true) {
        fk_module_tick(&module);

        delay(10);
    }
}
