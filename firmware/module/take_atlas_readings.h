#ifndef FK_TAKE_ATLAS_READINGS_H_INCLUDED
#define FK_TAKE_ATLAS_READINGS_H_INCLUDED

#include "atlas_fsm.h"

namespace fk {

constexpr size_t NumberOfAtlasReadings = 6
    #if defined(FK_ENABLE_ATLAS_EZO)
    + 1 // EC Has an extra value in EZO mode.
    #endif
    #if defined(FK_ENABLE_ATLAS_ORP)
    + 1
    #endif
    ;

constexpr size_t NumberOfReadings = NumberOfAtlasReadings
    #if defined(FK_ENABLE_MS5803)
    + 2
    #endif
    ;

class TakeAtlasReadings : public AtlasModuleState {
public:
    const char *name() const override {
        return "TakeAtlasReadings";
    }

public:
    void task() override;
};

}

#endif
