#include "pe1/gte.h"

void LoadAverageByte(void *first, void *second, int first_scale,
                     int second_scale, void * volatile output) {
    int lzcr;
    register unsigned char *out asm("$13");
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
    out = output;
    gte_getmac12_staged(out);
    gte_x >>= gte_shift;
    gte_y >>= gte_shift;
    out[0] = gte_x;
    out[1] = gte_y;
    asm volatile("" : : : "memory");
}
