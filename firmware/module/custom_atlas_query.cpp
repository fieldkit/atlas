#include "custom_atlas_query.h"

namespace fk {

constexpr uint32_t CustomAtlasCommandTimeout = 3000;

void CustomAtlasQuery::task() {
    auto& outgoing = services().child->outgoing();
    auto pool = *services().pool;
    auto query = services().query;

    fk_atlas_WireAtlasQuery queryMessage = fk_atlas_WireAtlasQuery_init_default;
    queryMessage.atlasCommand.command.funcs.decode = pb_decode_string;
    queryMessage.atlasCommand.command.arg = (void *)&pool;

    {
        auto raw = (pb_data_t *)query->m().custom.message.arg;
        auto stream = pb_istream_from_buffer((uint8_t *)raw->buffer, raw->length);
        if (!pb_decode(&stream, fk_atlas_WireAtlasQuery_fields, &queryMessage)) {
            log("Error decoding Atlas query (%d).", raw->length);
            return; // TaskEval::error();
        }
    }

    fk_atlas_WireAtlasReply replyMessage = fk_atlas_WireAtlasReply_init_default;
    replyMessage.type = fk_atlas_ReplyType_REPLY_ERROR;

    if (queryMessage.type == fk_atlas_QueryType_QUERY_ATLAS_COMMAND) {
        auto sensorPower = atlasServices().sensorPower;
        sensorPower->enable(AtlasCustomQueryMinimum);
        while (simple_task_run(*sensorPower)) {
            services().alive();
        }

        auto sensor = atlasServices().atlasSensors->getSensorByType(queryMessage.atlasCommand.sensor);
        auto command = (const char *)queryMessage.atlasCommand.command.arg;

        sensor->singleCommand(command);

        auto started = millis();
        while (millis() - started < CustomAtlasCommandTimeout && !sensor->isIdle()) {
            sensor->tick();
        }

        if (sensor->isIdle()) {
            replyMessage.type = fk_atlas_ReplyType_REPLY_ATLAS_COMMAND;
            replyMessage.atlasReply.reply.funcs.encode = pb_encode_string;
            replyMessage.atlasReply.reply.arg = (void *)sensor->lastReply();
        }
        else {
            log("Error getting reply from sensor");
        }
    }
    else {
        log("Unknown AtlasQuery");
    }

    size_t required = 0;

    if (!pb_get_encoded_size(&required, fk_atlas_WireAtlasReply_fields, &replyMessage)) {
        log("Error getting size for reply");
        transit<fk::ModuleIdle>();
        return;
    }

    auto messageData = pb_data_allocate(&pool, required);
    auto stream = pb_ostream_from_buffer((uint8_t *)messageData->buffer, messageData->length);
    if (!pb_encode(&stream, fk_atlas_WireAtlasReply_fields, &replyMessage)) {
        log("Error encoding Atlas reply");
        transit<fk::ModuleIdle>();
        return;
    }

    log("Replying with %d bytes", messageData->length);

    ModuleReplyMessage reply(pool);
    reply.m().type = fk_module_ReplyType_REPLY_CUSTOM;
    reply.m().custom.message.funcs.encode = pb_encode_data;
    reply.m().custom.message.arg = (void *)messageData;

    outgoing.write(reply);

    transit<fk::ModuleIdle>();
}

}
