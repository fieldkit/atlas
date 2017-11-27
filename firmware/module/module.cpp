/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller? Maybe with log levels?
 * TODO: Calibration mode. With the help of the app, this is a big deal I think.
 */
#include <Arduino.h>
#include <Wire.h>

#include "logging.h"
#include "leds.h"
#include "atlas.h"

using WireAddress = uint8_t;

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

Sensor sensors[] = {
    Sensor(ec),
    Sensor(ph),
    Sensor(dissolvedOxygen),
    Sensor(orp),
    Sensor(temp), // Sleep seems to work better with this at the end.
};

FkLeds leds;

SensorModule module(sensors);

void setup() {
    leds.setup();

    leds.on();

    Serial.begin(115200);

    Wire.begin();

    //  TODO: Investigate. I would see hangs if I used a slower speed.
    Wire.setClock(400000);

    while (!Serial) {
        delay(250);
    }

    module.setup();
}

void loop() {
    module.tick();

    if (module.numberOfReadingsReady() > 0) {
        float values[module.numberOfReadingsReady()];
        module.readAll(values);
    }
    if (module.isIdle()) {
        delay(10000);
        module.beginReading();
    }

    delay(10);
}
