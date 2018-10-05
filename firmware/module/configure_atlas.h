#ifndef FK_CONFIGURE_ATLAS_H_INCLUDED
#define FK_CONFIGURE_ATLAS_H_INCLUDED

#include "atlas_fsm.h"

namespace fk {

class ConfigureAtlas : public AtlasModuleState {
public:
    const char *name() const override {
        return "ConfigureAtlas";
    }

public:
    void task() override;
};

}

#endif
