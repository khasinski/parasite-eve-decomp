#include "pe1/gte.h"

void LoadAverageCol(void *first, void *second, int first_scale,
                    int second_scale, void * volatile output) {
    int lzcr;
    register unsigned char *out asm("$13");
    gte_declare_xyz_staging();
    gte_declare_shift12();

    gte_load_byte3(first);
    gte_ldir0_ir123(first_scale);
    gte_gpf0();
    gte_load_byte3(second);
    gte_getlzcr_now(lzcr);
    gte_ldir0_ir123(second_scale);
    gte_set_shift12();
    gte_gpl0_now();
    out = output;
    gte_getmac123_staged(out);
    gte_x >>= gte_shift;
    gte_y >>= gte_shift;
    gte_z >>= gte_shift;
    out[0] = gte_x;
    out[1] = gte_y;
    out[2] = gte_z;
    asm volatile("" : : : "memory");
}
