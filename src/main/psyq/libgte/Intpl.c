#include "pe1/gte.h"

void Intpl(void *input, int depth, void *output) {
    gte_ldir123v(input);
    gte_ldir0(depth);
    gte_intpl();
    gte_strgb(output);
}
