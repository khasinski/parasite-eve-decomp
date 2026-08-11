#include "pe1/gte.h"

void LoadAverageShort12(void *first, void *second, int first_scale,
                        int second_scale, void * volatile output) {
    int lzcr;
    gte_declare_xyz_staging();

    gte_load_packed_short3(first);
    gte_ldir0_ir123(first_scale);
    gte_gpf12();
    gte_load_packed_short3(second);
    gte_getlzcr_now(lzcr);
    gte_ldir0_ir123(second_scale);
    gte_gpl12();
    gte_store_ir123_packed_short3(output);
}
