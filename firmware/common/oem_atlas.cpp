#include "oem_atlas.h"
#include "debug.h"
#include "configuration.h"
#include "atlas_configuration.h"
#include "platform.h"

namespace fk {

constexpr char Log[] = "Atlas";

using Logger = SimpleLog<Log>;

struct AtlasRegisters {
    static constexpr uint8_t DeviceType = 0x00;
    static constexpr uint8_t Led = 0x05;
    static constexpr uint8_t Irq = 0x04;

    static constexpr uint8_t EcProbeType = 0x08;
};

constexpr uint8_t AtlasHigh = 1;
constexpr uint8_t AtlasLow = 0;

OemAtlas::OemAtlas(TwoWireBus &bus, uint8_t address) : bus_(&bus), address_(address) {
}

bool OemAtlas::setup() {
    return false;
}

bool OemAtlas::check() {
    TwoWire16 d16;

    bus_->send(address_, AtlasRegisters::DeviceType);
    bus_->receive(address_, d16);
    fk_delay(50);

    // Do this before we use the value to get the config.
    type_ = (AtlasSensorType)d16.bytes[1];

    auto cfg = config();
    if (!cfg.valid) {
        transition(State::Error);
        return false;
    }

    auto active = bus_->read(address_, cfg.active_register);
    fk_delay(50);

    auto irq = bus_->read(address_, AtlasRegisters::Irq);
    fk_delay(50);

    Logger::info("Atlas(0x%x, %s) -> (type = %d, version = %d) (active = %d) (irq = %d)", address_, config().name, d16.bytes[1], d16.bytes[0], active, irq);

    return true;
}

bool OemAtlas::wake() {
    Logger::info("Atlas(0x%x, %s) <- (wake)", address_, config().name);

    bus_->write(address_, config().active_register, AtlasHigh);

    return true;
}

bool OemAtlas::hibernate() {
    Logger::info("Atlas(0x%x, %s) <- (hibernate)", address_, config().name);

    bus_->write(address_, config().active_register, AtlasLow);

    return true;
}

bool OemAtlas::leds(bool on) {
    Logger::info("Atlas(0x%x, %s) <- (leds = %d)", address_, config().name, on);

    bus_->write(address_, AtlasRegisters::Led, on ? AtlasHigh : AtlasLow);

    return true;
}

bool OemAtlas::has_reading() {
    return bus_->read(address_, config().reading_register) == AtlasHigh;
}

void OemAtlas::transition(State state) {
    state_ = state;
    delay_end_ = fk_uptime() + 10;
}

TickSlice OemAtlas::tick() {
    if (delay_end_ > 0 && fk_uptime() < delay_end_) {
        return { };
    }

    delay_end_ = 0;

    switch (state_) {
    case State::Start: {
        if (!check()) {
            transition(State::Error);
            break;
        }

        switch (type_){
        case AtlasSensorType::Ph:
        case AtlasSensorType::Do:
        case AtlasSensorType::Temp:
        case AtlasSensorType::Orp:
            transition(State::LedsOff);
            break;
        case AtlasSensorType::Ec:
            transition(State::EcConfigureProbe);
            break;
        default:
            transition(State::Error);
            break;
        }

        break;
    }
    case State::EcConfigureProbe: {
        auto type = 0.1f;

        Logger::info("Atlas(0x%x, %s) <- (probe type = %f)", address_, config().name, type);

        if (!bus_->write(address_, AtlasRegisters::EcProbeType, (uint16_t)(type * 100))) {
            transition(State::Error);
        }
        else {
            transition(State::LedsOff);
        }

        break;
    }
    case State::ApplyCompensation: {
        transition(State::TakeReading);
        break;
    }
    case State::TakeReading: {
        auto cfg = config();

        if (!has_reading()) {
            break;
        }

        fk_delay(100);

        if (!bus_->send(address_, cfg.value_register)) {
            Logger::info("Atlas(0x%x, %s) Error 1", address_, cfg.name);
            transition(State::Error);
            break;
        }

        fk_delay(100);

        number_of_values_ = 0;

        for (auto i = 0; i < cfg.number_of_values; ++i) {
            TwoWire32 data = { 0 };

            if (!bus_->receive(address_, data)) {
                Logger::info("Atlas(0x%x, %s) Error 2", address_, cfg.name);
                transition(State::Error);
                break;
            }

            float value = data.u32;
            value /= cfg.divisor;

            Logger::info("Atlas(0x%x, %s) -> (value = %f) (raw = %lu)", address_, cfg.name, value, data.u32);

            if (number_of_values_ < ATLAS_MAXIMUM_NUMBER_OF_VALUES) {
                values_[number_of_values_++] = value;
            }
        }

        if (!bus_->write(address_, cfg.reading_register, AtlasLow)) {
            Logger::info("Atlas(0x%x, %s) Error 3", address_, cfg.name);
            transition(State::Error);
            break;
        }

        if (state_ == State::Error) {
            break;
        }

        if (sleep_after_) {
            transition(State::LedsOff);
        }
        else {
            transition(State::Idle);
        }
        break;
    }
    case State::LedsOn: {
        if (!leds(true)) {
            transition(State::Error);
        }
        else {
            transition(State::ApplyCompensation);
        }
        break;
    }
    case State::LedsOff: {
        if (!leds(false)) {
            transition(State::Error);
        }
        else {
            transition(State::Hibernate);
        }
        break;
    }
    case State::Wake: {
        if (!wake()) {
            transition(State::Error);
        }
        else {
            transition(State::LedsOn);
        }
        break;
    }
    case State::Hibernate: {
        if (!hibernate()) {
            transition(State::Error);
        }
        else {
            transition(State::Hibernating);
        }
        break;
    }
    case State::Error: {
        auto cfg = config();
        Logger::info("Atlas(0x%x, %s) ERROR", address_, cfg.name);
        delay_end_ = fk_uptime() + 1000;
        break;
    }
    case State::Idle: {
        break;
    }
    case State::Hibernating: {
        break;
    }
    }

    return { };
}

bool OemAtlas::beginReading(bool sleep) {
    sleep_after_ = sleep;
    switch (state_) {
    case State::Idle:
        transition(State::TakeReading);
        break;
    case State::Hibernating:
        transition(State::Wake);
        break;
    }
    return true;
}

size_t OemAtlas::readAll(float *values) {
    for (size_t i = 0; i < number_of_values_; ++i) {
        *values++ = values_[i];
    }
    auto number = number_of_values_;
    number_of_values_ = 0;
    return number;
}

size_t OemAtlas::numberOfReadingsReady() const {
    return number_of_values_;
}

bool OemAtlas::isIdle() const {
    return state_ == State::Hibernating || state_ == State::Idle;
}

bool OemAtlas::singleCommand(const char *command) {
    return false;
}

OemAtlas::Config OemAtlas::config() {
    switch (type_) {
    case AtlasSensorType::Ec: {
        return { true, 0x18, 0x07, 0x06, 3, 100.0f, "EC" };
    }
    case AtlasSensorType::Ph: {
        return { true, 0x16, 0x07, 0x06, 1, 1000.0f, "PH" };
    }
    case AtlasSensorType::Do: {
        return { true, 0x22, 0x07, 0x06, 1, 100.0f, "DO" };
    }
    case AtlasSensorType::Temp: {
        return { true, 0x0E, 0x07, 0x06, 1, 1000.0f, "TEMP" };
    }
    case AtlasSensorType::Orp: {
        return { true, 0x0E, 0x07, 0x06, 1, 10.0f, "ORP" };
    }
    }

    return { false, 0, 0, 0, 0, 1.0f, "<unknown>" };
}

}
