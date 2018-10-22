#ifndef FK_ATLAS_CONFIGURATION_H_INCLUDED
#define FK_ATLAS_CONFIGURATION_H_INCLUDED

#include <cinttypes>

namespace fk {

struct AtlasConfiguration {
    /**
     *
     */
    uint8_t maximum_atlas_retries = 5;
};
/**
 *
 */
extern const AtlasConfiguration atlas_configuration;

}

#endif
