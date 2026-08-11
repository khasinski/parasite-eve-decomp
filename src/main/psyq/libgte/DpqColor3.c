#include "pe1/gte.h"

void DpqColor3(void *rgb0, void *rgb1, void *rgb2, int interpolation,
               void * volatile out0, void * volatile out1,
               void * volatile out2) {
    gte_declare_three_outputs();

    gte_ldrgb0(rgb0);
    gte_ldrgb1(rgb1);
    gte_ldrgb2(rgb2);
    gte_ldrgb(rgb2);
    gte_ldir0(interpolation);
    gte_dpct_separate();
    gte_bind_three_outputs(out0, out1, out2);
    gte_strgb0_bound();
    gte_strgb1_bound();
    gte_strgb2_bound();
}
