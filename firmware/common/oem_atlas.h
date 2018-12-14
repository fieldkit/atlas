#ifndef FK_OEM_ATLAS_H_INCLUDED
#define FK_OEM_ATLAS_H_INCLUDED

#include "atlas_common.h"

namespace fk {

class OemAtlas {
public:
    static constexpr uint8_t EC_DEFAULT_ADDRESS = 0x64;
    static constexpr uint8_t TEMP_DEFAULT_ADDRESS = 0x68;
    static constexpr uint8_t PH_DEFAULT_ADDRESS = 0x65;
    static constexpr uint8_t DO_DEFAULT_ADDRESS = 0x67;
    static constexpr uint8_t ORP_DEFAULT_ADDRESS = 0x66;

private:
    enum class State {
        Start,
        EcConfigureProbe,
        ApplyCompensation,
        TakeReading,
        LedsOff,
        Hibernate,
        Wake,
        Idle,
        Hibernating,
        Error,
    };

private:
    TwoWireBus *bus_;
    uint8_t address_{ 0 };
    AtlasSensorType type_{ AtlasSensorType::Unknown };
    bool sleep_after_{ false };
    Compensation compensation_;
    State state_{ State::Start };
    uint32_t delay_end_{ 0 };

public:
    OemAtlas(TwoWireBus &bus, uint8_t address);

public:
    bool setup();
    TickSlice tick();
    bool isIdle() const;

public:
    bool beginReading(bool sleep);
    size_t readAll(float *values);
    size_t numberOfReadingsReady() const;
    bool singleCommand(const char *command);
    void compensate(Compensation c) {
        compensation_ = c;
    }
    const char *lastReply() {
        return "";
    }

public:
    bool check();
    bool wake();
    bool hibernate();
    bool leds(bool on);
    bool has_reading();

private:
    struct Config {
        uint8_t valid;
        uint8_t value_register;
        uint8_t reading_register;
        uint8_t active_register;
        uint8_t number_of_values;
        float divisor;
        const char *name;
    };

    Config config();

    void transition(State state);

};

}

#endif
