#ifndef FK_ATLAS_HARDWARE_H_INCLUDED
#define FK_ATLAS_HARDWARE_H_INCLUDED

#include <cinttypes>

#include "board_definition.h"

namespace fk {

/**
 * Minimum time we can enable the peripherals for. This is to avoid Atlas module
 * EEPROM corruption.
 */
constexpr uint32_t FK_ATLAS_PERIPH_MINIMUM_ENABLE_TIME = 1000;

}

#endif

