/**
 * Things to keep in mind:
 * TODO: Allow calibration based on temperature reading (if attached)
 * TODO: Logging that can be relayed to the caller? Maybe with log levels?
 * TODO: Calibration mode. With the help of the app, this is a big deal I think.
 */
#include <Arduino.h>
#include <Wire.h>

#include <fk-module.h>
#include <fk-atlas-protocol.h>

#include "atlas.h"

namespace fk {

using WireAddress = uint8_t;
using PinNumber = uint8_t;

const WireAddress ATLAS_SENSOR_EC_DEFAULT_ADDRESS = 0x64;
const WireAddress ATLAS_SENSOR_TEMP_DEFAULT_ADDRESS = 0x66;
const WireAddress ATLAS_SENSOR_PH_DEFAULT_ADDRESS = 0x63;
const WireAddress ATLAS_SENSOR_DO_DEFAULT_ADDRESS = 0x61;
const WireAddress ATLAS_SENSOR_ORP_DEFAULT_ADDRESS = 0x62;

class AtlasCommandTask : public Task {
private:
    const char *command;
    ModuleReplyMessage *reply;

public:
    AtlasCommandTask(const char *command) : Task("Atlas"), command(command) {
    }

public:
    TaskEval task() override;

};

TaskEval AtlasCommandTask::task() {
    return TaskEval::done();
}

class AtlasModule : public Module {
private:
    Pool pool{ "AtlasModule", 128 };
    SensorModule *atlasSensors;
    TwoWireBus bus{ Wire11and13 };

public:
    AtlasModule(ModuleInfo &info, SensorModule &atlasSensors);

public:
    ModuleReadingStatus beginReading(PendingSensorReading &pending) override;
    ModuleReadingStatus readingStatus(PendingSensorReading &pending) override;
    TaskEval message(ModuleQueryMessage &query, ModuleReplyMessage &reply) override;
};

AtlasModule::AtlasModule(ModuleInfo &info, SensorModule &atlasSensors) : Module(bus, info), atlasSensors(&atlasSensors) {
}

ModuleReadingStatus AtlasModule::beginReading(PendingSensorReading &pending) {
    atlasSensors->beginReading();

    return ModuleReadingStatus{ 1000 };
}

ModuleReadingStatus AtlasModule::readingStatus(PendingSensorReading &pending) {
    debugfpln("Atlas", "Number: %d", atlasSensors->numberOfReadingsReady());
    if (atlasSensors->numberOfReadingsReady() == 8) {
        // Order: Ec1,2,3,4,Temp,pH,Do,ORP
        float values[atlasSensors->numberOfReadingsReady()];
        size_t size = atlasSensors->readAll(values);

        auto readings = pending.readings;
        readings[ 0].value = values[0];
        readings[ 1].value = values[1];
        readings[ 2].value = values[2];
        readings[ 3].value = values[3];
        readings[ 4].value = values[4];
        readings[ 5].value = values[5];
        readings[ 6].value = values[6];
        readings[ 7].value = values[7];
        pending.elapsed -= millis();
        for (auto i = 0; i < 8; ++i) {
            readings[i].status = SensorReadingStatus::Done;
            readings[i].time = clock.getTime();
        }
    }

    return ModuleReadingStatus{};
}

TaskEval AtlasModule::message(ModuleQueryMessage &query, ModuleReplyMessage &reply) {
    pool.clear();

    fk_atlas_WireAtlasQuery queryMessage = fk_atlas_WireAtlasQuery_init_default;
    queryMessage.atlasCommand.command.funcs.decode = pb_decode_string;
    queryMessage.atlasCommand.command.arg = (void *)&pool;

    {
        auto raw = (pb_data_t *)query.m().custom.message.arg;
        auto stream = pb_istream_from_buffer((uint8_t *)raw->buffer, raw->length);
        if (!pb_decode_delimited(&stream, fk_atlas_WireAtlasQuery_fields, &queryMessage)) {
            return TaskEval::error();
        }
    }

    fk_atlas_WireAtlasReply replyMessage = fk_atlas_WireAtlasReply_init_default;
    replyMessage.type = fk_atlas_ReplyType_REPLY_ERROR;

    if (queryMessage.type == fk_atlas_QueryType_QUERY_ATLAS_COMMAND) {
        Sensor *sensor = nullptr;

        switch (queryMessage.atlasCommand.sensor) {
        case fk_atlas_SensorType_PH: {
            sensor = atlasSensors->getSensor(1);
            break;
        }
        case fk_atlas_SensorType_TEMP: {
            sensor = atlasSensors->getSensor(4);
            break;
        }
        case fk_atlas_SensorType_ORP: {
            sensor = atlasSensors->getSensor(3);
            break;
        }
        case fk_atlas_SensorType_DO: {
            sensor = atlasSensors->getSensor(2);
            break;
        }
        case fk_atlas_SensorType_EC: {
            sensor = atlasSensors->getSensor(0);
            break;
        }
        }

        auto reader = reinterpret_cast<AtlasReader*>(sensor);

        reader->singleCommand((const char *)queryMessage.atlasCommand.command.arg);

        auto started = millis();
        while (millis() - started < 1000 && !reader->isIdle()) {
            reader->tick();
        }

        if (!reader->isIdle()) {
            replyMessage.type = fk_atlas_ReplyType_REPLY_ATLAS_COMMAND;
            replyMessage.atlasReply.reply.funcs.encode = pb_encode_string;
            replyMessage.atlasReply.reply.arg = (void *)reader->lastReply();
        }
    }

    size_t required = 0;

    if (!pb_get_encoded_size(&required, fk_atlas_WireAtlasReply_fields, &replyMessage)) {
        return TaskEval::error();
    }

    auto messageData = pb_data_allocate(&pool, required + ProtoBufEncodeOverhead);
    auto stream = pb_ostream_from_buffer((uint8_t *)messageData->buffer, messageData->length);
    if (!pb_encode_delimited(&stream, fk_atlas_WireAtlasReply_fields, &replyMessage)) {
        return TaskEval::error();
    }

    reply.m().type = fk_module_ReplyType_REPLY_CUSTOM;
    reply.m().custom.message.funcs.encode = pb_encode_data;
    reply.m().custom.message.arg = (void *)messageData;

    return TaskEval::done();
}

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
            {"pH", "",},
            {"DO", "mg/L",},
            {"ORP", "mV",},
            {"Temp", "C",},
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
