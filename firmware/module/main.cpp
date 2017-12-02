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
FkLeds leds;

void flush(Stream &stream) {
    if (stream.available()) {
        auto flushed = 0;
        while (stream.available()) {
            flushed++;
            stream.read();
        }
        debugfln("i2c: flushed %d bytes", flushed);
    }
}

fk::SensorInfo mySensors[] = {
    {
        .sensor = 0,
        .name = "Ec",
        .unitOfMeasure = "µS/cm",
    },
    {
        .sensor = 1,
        .name = "TDS",
        .unitOfMeasure = "°ppm",
    },
    {
        .sensor = 2,
        .name = "Salinity",
        .unitOfMeasure = "",
    },
    {
        .sensor = 3,
        .name = "SG",
        .unitOfMeasure = "",
    },
    {
        .sensor = 4,
        .name = "Temp",
        .unitOfMeasure = "C",
    },
    {
        .sensor = 5,
        .name = "pH",
        .unitOfMeasure = "",
    },
    {
        .sensor = 6,
        .name = "DO",
        .unitOfMeasure = "mg/L",
    },
    {
        .sensor = 7,
        .name = "ORP",
        .unitOfMeasure = "mV",
    },
};

fk::ModuleInfo myInfo = {
    .address = 8,
    .numberOfSensors = 8,
    .name = "Atlas",
    .sensors = mySensors,
};

class ExampleModule : public fk::Module {
private:

public:
    ExampleModule();

public:
    void beginReading() override;
    void readingDone() override;
    void describeSensor(size_t number) override;
};

ExampleModule::ExampleModule() : Module(myInfo) {
}

void ExampleModule::beginReading() {
    readingDone();
}

void ExampleModule::readingDone() {
}

void ExampleModule::describeSensor(size_t number) {
    switch (number) {
    }
}

extern "C" {

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        delay(100);
    }

    debugfpln("Module", "Starting (%d free)", fk_free_memory());

    ExampleModule module;

    module.begin();

    Wire.begin();
    // TODO: Investigate. I would see hangs if I used a slower speed.
    Wire.setClock(400000);

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

                module.resume();

                flush(Wire);
            }
            else {
                module.tick();
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
}

}
