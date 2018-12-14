#ifndef FK_ATLAS_COMMON_H_INCLUDED
#define FK_ATLAS_COMMON_H_INCLUDED

#include <functional>

#include <two_wire.h>

namespace fk {

enum class AtlasSensorType : uint8_t {
    Unknown = 0xff,
    Ph = 1,
    Ec = 4,
    Orp = 2,
    Do = 3,
    Temp = 5
};

struct Compensation {
    float temperature;
    bool valid;

    Compensation() {
    }

    Compensation(float temperature) : temperature(temperature), valid(true) {
    }

    operator bool() {
        return valid && temperature >= -1000.0f;
    }
};

struct TickSlice {
    bool waitingOnSiblings{ false };
    std::function<void()> onFree;

    TickSlice() {
    }

    TickSlice(std::function<void()> f) : waitingOnSiblings(true), onFree(f) {
    }

    void free() {
        onFree();
    }
};

}

#endif
