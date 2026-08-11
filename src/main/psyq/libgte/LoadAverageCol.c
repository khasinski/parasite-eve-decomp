#include "pe1/gte.h"

void LoadAverageCol(void *first, void *second, int first_scale,
                    int second_scale, void * volatile output) {
    int lzcr;
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
    gte_store_mac123_byte3(output);
}
