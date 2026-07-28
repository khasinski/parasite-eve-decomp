#include "pe1/gte.h"

void DpqColorLight(void *input, void *color, int depth, void *output) {
    gte_ldir123v(input);
    gte_ldrgb(color);
    gte_ldir0(depth);
    gte_dpcl();
    gte_strgb(output);
}
