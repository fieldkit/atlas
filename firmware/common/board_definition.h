#ifndef FK_BOARD_DEFINITION_H_INCLUDED
#define FK_BOARD_DEFINITION_H_INCLUDED

#include "board.h"
#include "hardware.h"

namespace fk {

constexpr uint8_t FK_ATLAS_PIN_FLASH_CS = 5;
constexpr uint8_t FK_ATLAS_PIN_PERIPH_ENABLE = 12;
constexpr uint8_t FK_ATLAS_PIN_ATLAS_ENABLE = 6;

class AtlasBoard : public Board {
private:
    PowerSwitch atlas_power_;

public:
    AtlasBoard(BoardConfig config);

public:
    void enable_atlas_modules();
    void disable_atlas_modules();

};

extern AtlasBoard board;

}

#endif
