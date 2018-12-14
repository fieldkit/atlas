#include "board_definition.h"

#include <Wire.h>
namespace fk {

AtlasBoard::AtlasBoard(BoardConfig config) : Board(config) {
}

void AtlasBoard::enable_atlas_modules() {
    high(FK_ATLAS_PIN_ATLAS_ENABLE);
    i2c1().begin();
}

void AtlasBoard::disable_atlas_modules() {
    low(FK_ATLAS_PIN_ATLAS_ENABLE);
    i2c1().end();
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
