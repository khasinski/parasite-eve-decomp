#include "pe1/gte.h"

void LoadAverageShort12(void *first, void *second, int first_scale,
                        int second_scale, void * volatile output) {
    int lzcr;
    register int *out asm("$13");
    gte_declare_xyz_staging();

    gte_x = ((int *)first)[0];
    gte_z = ((volatile int *)first)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    gte_ldir0_ir123(first_scale);
    gte_gpf12();
    gte_x = ((int *)second)[0];
    gte_z = ((volatile int *)second)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    gte_getlzcr_now(lzcr);
    gte_ldir0_ir123(second_scale);
    gte_gpl12();
    gte_mfc2_9(gte_x);
    gte_mfc2_10(gte_y);
    gte_x &= 0xFFFF;
    gte_y = (unsigned int)gte_y << 16;
    gte_x |= gte_y;
    out = output;
    gte_mfc2_11(gte_z);
    ((volatile int *)out)[0] = gte_x;
    ((volatile int *)out)[1] = gte_z;
}
