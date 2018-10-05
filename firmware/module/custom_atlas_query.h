#ifndef FK_CUSTOM_ATLAS_QUERY_H_INCLUDED
#define FK_CUSTOM_ATLAS_QUERY_H_INCLUDED

#include "atlas_fsm.h"

namespace fk {

class CustomAtlasQuery : public AtlasModuleState {
public:
    const char *name() const override {
        return "CustomAtlasQuery";
    }

public:
    void task() override;
};

}

#endif
