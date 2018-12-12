#include <alogging/alogging.h>

#include "board_definition.h"

namespace fk {

constexpr const char Log[] = "Hardware";

using Logger = SimpleLog<Log>;

AtlasBoard::AtlasBoard(BoardConfig config) : Board(config) {
}

void AtlasBoard::enable_atlas_modules() {
    if (atlas_power_.take()) {
        Logger::info("Atlas on");
        high(FK_ATLAS_PIN_ATLAS_ENABLE);
        i2c1().begin();
    }
}

void AtlasBoard::disable_atlas_modules() {
    if (atlas_power_.release()) {
        Logger::info("Atlas off");
        low(FK_ATLAS_PIN_ATLAS_ENABLE);
        i2c1().end();
    }
}

AtlasBoard board{
    {
        FK_ATLAS_PIN_PERIPH_ENABLE,
        FK_ATLAS_PIN_FLASH_CS,
        {
            FK_ATLAS_PIN_FLASH_CS,
            0,
            0,
            0,
        },
        {
            FK_ATLAS_PIN_PERIPH_ENABLE,
            FK_ATLAS_PIN_ATLAS_ENABLE,
            0,
            0,
        }
    }
};

}
