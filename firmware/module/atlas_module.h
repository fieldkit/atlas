#ifndef FK_ATLAS_MODULE_H_INCLUDED
#define FK_ATLAS_MODULE_H_INCLUDED

#include <fk-module.h>
#include "atlas.h"

namespace fk {

class AtlasModule : public Module {
private:
    Pool pool{ "AtlasModule", 128 };
    SensorModule *atlasSensors;
    TwoWireBus bus{ Wire11and13 };

public:
    AtlasModule(ModuleInfo &info, SensorModule &atlasSensors);

public:
    ModuleReadingStatus beginReading(PendingSensorReading &pending) override;
    ModuleReadingStatus readingStatus(PendingSensorReading &pending) override;
    TaskEval message(ModuleQueryMessage &query, ModuleReplyMessage &reply) override;
};

}

#endif
