#ifndef COLORS_HPP
#define COLORS_HPP

#include <FL/Fl.H>

// FLTK Colors in hex
// used in cell_content and shape
static constexpr Fl_Color flRelative[7] = {
    0x22a0fd00,  // Blue
    0x4ad81200,  // Green
    0xfe810200,  // Orange
    0xd31ded00,  // Purple
    0xe3010200,  // Red
    0xFFFF8A00,  // Yellow
    0xAD8A6400   // Brown
};

// Cell
const unsigned CELL_CLR = 0x495D6A00;
const unsigned CELL_SELECT_CLR =0xA7B8C300 ; // 0xffbf0000
const unsigned CELL_HOVER_CLR =0x718C9E00 ; // 0xffbf0000

#endif
