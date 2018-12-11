#include "board_definition.h"

namespace fk {

Board<BoardConfig<1, 2>> board{
    {
        FK_ATLAS_PIN_PERIPH_ENABLE,
        FK_ATLAS_PIN_FLASH_CS,
        {
            FK_ATLAS_PIN_FLASH_CS,
        },
        {
            FK_ATLAS_PIN_PERIPH_ENABLE,
            FK_ATLAS_PIN_ATLAS_ENABLE,
        }
    }
};

}
