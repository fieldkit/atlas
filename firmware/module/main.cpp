/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller? Maybe with log levels?
 * TODO: Calibration mode. With the help of the app, this is a big deal I think.
 */
#include <Arduino.h>
#include <Wire.h>

#include <fk-module.h>

#include "leds.h"
#include "atlas.h"

using WireAddress = uint8_t;
using PinNumber = uint8_t;

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
fk::Leds leds;

void flush(Stream &stream) {
    if (stream.available()) {
        auto flushed = 0;
        while (stream.available()) {
            flushed++;
            stream.read();
        }
        debugfpln("I2C", "Flushed %d bytes", flushed);
    }
}

class AtlasModule : public fk::Module {
private:
    fk::TwoWireBus bus{ fk::Wire11and13 };

public:
    AtlasModule(fk::ModuleInfo &info);

public:
    fk::ModuleReadingStatus beginReading(fk::PendingSensorReading &pending) override;

};

AtlasModule::AtlasModule(fk::ModuleInfo &info) : Module(bus, info) {
}

fk::ModuleReadingStatus AtlasModule::beginReading(fk::PendingSensorReading &pending) {
    return fk::ModuleReadingStatus();
}

extern "C" {

void setup() {
    Serial.begin(115200);

    while (!Serial) {
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

    AtlasModule module(info);

    module.begin();

    Wire.begin();
    // TODO: Investigate. I would see hangs if I used a slower speed.
    Wire.setClock(400000);

    uint32_t idleStart = 0;

    while (true) {
        sensorModule.tick();
        module.tick();

        if (sensorModule.numberOfReadingsReady() > 0) {
            // Order: Ec1,2,3,4,Temp,pH,Do,ORP
            float values[sensorModule.numberOfReadingsReady()];
            size_t size = sensorModule.readAll(values);
        }

        if (sensorModule.isIdle()) {
            if (idleStart == 0 ) {
                idleStart = millis() + 20000;

                // module.resume();

                flush(Wire);
            }
            else {
                // module.tick();
            }

            if (idleStart < millis()) {
                // Wire.begin();
                // TODO: Investigate. I would see hangs if I used a slower speed.
                // Wire.setClock(400000);

                sensorModule.beginReading();
                idleStart = 0;
            }
        }

        delay(10);
    }
}

void loop() {
}

}
