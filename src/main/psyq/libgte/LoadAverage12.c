#include "pe1/gte.h"

void LoadAverage12(void *first, void *second, int first_scale, int second_scale,
                   void * volatile output) {
    int lzcr;

    gte_ldir0(first_scale);
    gte_ldir123v(first);
    gte_gpf12();
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_ldir123v(second);
    gte_gpl12();
    gte_stir123_gpl(output);
}
