#include "board_definition.h"

namespace fk {

Board board{
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
