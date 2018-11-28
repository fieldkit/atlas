#ifndef FK_ATLAS_HARDWARE_H_INCLUDED
#define FK_ATLAS_HARDWARE_H_INCLUDED

#include <cinttypes>

namespace fk {

constexpr uint8_t FK_ATLAS_PIN_FLASH = 5;
constexpr uint8_t FK_ATLAS_PIN_PERIPH_ENABLE = 12;
constexpr uint8_t FK_ATLAS_PIN_ATLAS_ENABLE = 6;

/**
 * Minimum time we can enable the peripherals for. This is to avoid Atlas module
 * EEPROM corruption.
 */
constexpr uint32_t FK_ATLAS_PERIPH_MINIMUM_ENABLE_TIME = 1000;

}

#endif

