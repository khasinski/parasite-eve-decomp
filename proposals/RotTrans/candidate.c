#include "pe1/gte.h"
#include "pe1/gte_types.h"

void RotTrans(const GteShortVector *v, GteVector *out, s32 *flag) {
    int flagValue;
    gte_lwc2_0_0(v);
    gte_lwc2_1_4(v);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(out);
    gte_swc2_26_4(out);
    gte_swc2_27_8(out);
    gte_getflag(flagValue);
    *flag = flagValue;
}

static unsigned int tail_alignment_69AFC[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000 };
