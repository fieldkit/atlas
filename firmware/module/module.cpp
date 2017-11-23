/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller?
 */
#include <Arduino.h>
#include <Wire.h>

class SensorReader {
public:
    virtual bool setup() = 0;
    virtual bool tick() = 0;
};

class AtlasReader : public SensorReader {
private:
    TwoWire *bus;
    uint8_t address;

public:
    AtlasReader(TwoWire *bus, uint8_t address) : bus(bus), address(address) {
    }

    bool setup() override {
        return true;
    }

    bool tick() override {
        return true;
    }
};

class Sensor {
private:
    SensorReader &reader;
    bool available;
    bool readingReady;

public:
    Sensor(SensorReader &reader) : reader(reader) {
    }

    bool isAvailable() {
        return available;
    }

    bool hasReadingReady() {
        return readingReady;
    }

    bool setup() {
        available = true;
        return true;
    }

    bool tick() {
        return true;
    }

    void beginTakeReading() {
        readingReady = true;
    }
};

class SensorModule {
private:
    Sensor *sensors;
    const size_t numberOfSensors;

public:
    template<size_t N>
    SensorModule(Sensor (&sensors)[N]) : sensors(sensors), numberOfSensors(N) {
    }

    bool setup(bool requireAllSensors) {
        for (size_t i = 0; i < numberOfSensors; ++i) {
            if (!sensors[i].setup() && requireAllSensors) {
                return false;
            }
        }
        return true;
    }

    bool tick() {
        for (size_t i = 0; i < numberOfSensors; ++i) {
            sensors[i].tick();
        }
        return true;
    }

    void beginTakeReading() {
        for (size_t i = 0; i < numberOfSensors; ++i) {
            sensors[i].beginTakeReading();
        }
    }

    bool hasReadingReady() {
        for (size_t i = 0; i < numberOfSensors; ++i) {
            if (!sensors[i].hasReadingReady()) {
                return false;
            }
        }
        return true;
    }
};

const uint8_t ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const uint8_t ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const uint8_t ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const uint8_t ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const uint8_t ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

const uint8_t ATLAS_RESPONSE_CODE_NO_DATA = 0xff;
const uint8_t ATLAS_RESPONSE_CODE_NOT_READY = 0xfe;
const uint8_t ATLAS_RESPONSE_CODE_ERROR = 0x2;
const uint8_t ATLAS_RESPONSE_CODE_SUCCESS = 0x1;

AtlasReader ph(&Wire, ATLAS_SENSOR_PH_DEFAULT_ADDRESS);

Sensor sensors[] = {
    Sensor(ph),
    Sensor(ph),
    Sensor(ph),
    Sensor(ph),
};

SensorModule module(sensors);

void setup() {
    Wire.begin();

    module.setup(true);
}

void loop() {
    module.tick();
}
