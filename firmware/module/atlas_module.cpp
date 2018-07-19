#include "atlas_module.h"

namespace fk {

constexpr uint32_t CustomAtlasCommandTimeout = 3000;

AtlasModule::AtlasModule(ModuleInfo &info, TwoWireBus &sensorBus) : Module(moduleBus, info),
    sensorBus(&sensorBus), sensors {
        &ec,
        &ph,
        &dissolvedOxygen,
        #ifdef FK_ENABLE_ATLAS_ORP
        &orp,
        #endif
        &temp
    }, atlasSensors(sensors) {
}

void AtlasModule::begin() {
    Module::begin();

    atlasSensors.setup();

    #ifdef FK_ENABLE_MS5803
    ms5803Pressure.reset();
    ms5803Pressure.begin();
    #endif

    taskQueue().append(enableSensors);
    taskQueue().append(atlasSensors);
}

void AtlasModule::tick() {
    Module::tick();

    if (elapsedSinceActivity() > 5000) {
        if (enableSensors.enabled()) {
            log("Disabling peripherals.");
            enableSensors.enabled(false);
        }
    }
}

ModuleReadingStatus AtlasModule::beginReading(PendingSensorReading &pending) {
    atlasSensors.beginReading(pending.number <= 1);

    taskQueue().append(enableSensors);
    taskQueue().append(atlasSensors);

    if (enableSensors.enabled()) {
        return ModuleReadingStatus{ 1000 };
    }

    return ModuleReadingStatus{ 3000 };
}

ModuleReadingStatus AtlasModule::readingStatus(PendingSensorReading &pending) {
    static_assert(NumberOfReadings >= NumberOfAtlasReadings, "NumberOfReadings should be >= NumberOfAtlasReadings");

    log("NumberOfReadingsReady: %d", atlasSensors.numberOfReadingsReady());

    if (atlasSensors.numberOfReadingsReady() == NumberOfAtlasReadings) {
        // Order: Ec1,2,3,4,pH,Do,ORP,Temp
        float values[atlasSensors.numberOfReadingsReady()];
        auto size = atlasSensors.readAll(values);
        auto now = clock.getTime();

        auto readings = pending.readings;
        for (auto i = 0; i < size; ++i) {
            readings[i].value = values[i];
            readings[i].status = SensorReadingStatus::Done;
            readings[i].time = now;
        }

        #ifdef FK_ENABLE_MS5803
        auto pressureTemperature = ms5803Pressure.getTemperature(CELSIUS, ADC_512);
        auto pressureAbsolute = ms5803Pressure.getPressure(ADC_4096);
        auto index = size;

        readings[index].value = pressureTemperature;
        readings[index].status = SensorReadingStatus::Done;
        readings[index].time = now;

        index++;

        readings[index].value = pressureAbsolute;
        readings[index].status = SensorReadingStatus::Done;
        readings[index].time = now;

        log("Ms5803: %f %f (%d)", pressureAbsolute, pressureTemperature, index);
        #endif

        pending.elapsed -= millis();
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
        if (!pb_decode(&stream, fk_atlas_WireAtlasQuery_fields, &queryMessage)) {
            log("Error decoding Atlas query (%d).", raw->length);
            return TaskEval::error();
        }
    }

    fk_atlas_WireAtlasReply replyMessage = fk_atlas_WireAtlasReply_init_default;
    replyMessage.type = fk_atlas_ReplyType_REPLY_ERROR;

    if (queryMessage.type == fk_atlas_QueryType_QUERY_ATLAS_COMMAND) {
        auto sensor = getSensor(queryMessage.atlasCommand.sensor);
        auto command = (const char *)queryMessage.atlasCommand.command.arg;

        sensor.singleCommand(command);

        auto started = millis();
        while (millis() - started < CustomAtlasCommandTimeout && !sensor.isIdle()) {
            sensor.tick();
        }

        if (sensor.isIdle()) {
            replyMessage.type = fk_atlas_ReplyType_REPLY_ATLAS_COMMAND;
            replyMessage.atlasReply.reply.funcs.encode = pb_encode_string;
            replyMessage.atlasReply.reply.arg = (void *)sensor.lastReply();
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
        return TaskEval::error();
    }

    auto messageData = pb_data_allocate(&pool, required);
    auto stream = pb_ostream_from_buffer((uint8_t *)messageData->buffer, messageData->length);
    if (!pb_encode(&stream, fk_atlas_WireAtlasReply_fields, &replyMessage)) {
        log("Error encoding Atlas reply");
        return TaskEval::error();
    }

    log("Replying with %d bytes", messageData->length);

    reply.m().type = fk_module_ReplyType_REPLY_CUSTOM;
    reply.m().custom.message.funcs.encode = pb_encode_data;
    reply.m().custom.message.arg = (void *)messageData;

    return TaskEval::done();
}

AtlasReader &AtlasModule::getSensor(fk_atlas_SensorType type) {
    switch (type) {
    case fk_atlas_SensorType_PH: return ph;
    case fk_atlas_SensorType_TEMP: return temp;
    #ifdef FK_ENABLE_ATLAS_ORP
    case fk_atlas_SensorType_ORP: return orp;
    #endif
    case fk_atlas_SensorType_DO: return dissolvedOxygen;
    case fk_atlas_SensorType_EC: return ec;
    }

    fk_assert(false);
}

}
