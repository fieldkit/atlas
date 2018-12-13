#ifndef FK_ATLAS_COMMON_H_INCLUDED
#define FK_ATLAS_COMMON_H_INCLUDED

#include <functional>

#include <two_wire.h>

namespace fk {

enum class AtlasSensorType {
    Unknown,
    Ph,
    Ec,
    Orp,
    Do,
    Temp
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
