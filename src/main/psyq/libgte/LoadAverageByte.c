#include "pe1/gte.h"

void LoadAverageByte(void *first, void *second, int first_scale,
                     int second_scale, void * volatile output) {
    int lzcr;
    gte_declare_xy_staging();
    gte_declare_shift12();

    gte_load_byte2(first);
    gte_ldir0_ir12(first_scale);
    gte_gpf0();
    gte_load_byte2(second);
    gte_getlzcr_now(lzcr);
    gte_ldir0_ir12(second_scale);
    gte_set_shift12();
    gte_gpl0_now();
    gte_store_mac12_byte2(output);
}
