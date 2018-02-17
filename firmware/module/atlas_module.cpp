#include <fk-atlas-protocol.h>

#include "atlas_module.h"

namespace fk {

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
