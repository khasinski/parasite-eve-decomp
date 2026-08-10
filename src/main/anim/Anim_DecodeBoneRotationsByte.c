#include "common.h"
#include "pe1/render_object.h"
void RotMatrixYXZ();

void Anim_DecodeBoneRotationsByte(RenderObjectEntity *arg0, RenderAnimationDataHeader *arg1, s16 arg2) {
    u16 *var_s3;
    s32 var_s2;
    s32 var_v0;
    s32 nlim;
    s32 *var_a1;
    s32 temp_v1;
    s32 temp_s5;
    s32 temp_s6;
    char *var_s4;
    u8 var_v0_2;
    u8 var_v0_3;
    u8 var_v0_4;
    char *var_s0;
    u16 *var_s1;
    char *var_v1;
    char *var_a0;

    var_s3 = (u16 *)0x1F800000;
    var_s0 = (char *)(arg1 + 1);
    var_s2 = 0;
    var_s4 = (char *)arg0->matrices;
    arg0->active_matrix = (RenderMatrix *)var_s4;
    var_a1 = (s32 *) (var_s4 + 0x14);
    temp_v1 = (arg1->packing_flags >> 1) + 1;
    temp_v1 = temp_v1 * 4;
    do {
        var_a0 = var_s0 + 2;
        if (((RenderAnimShortChannel *)var_s0)->constant_marker != 0) {
            var_v0 = ((RenderAnimShortChannel *)var_s0)->samples.signed_values[0];
            var_s0 += 4;
        } else {
            var_v0 = *(s16 *)((((s32) (arg2 << 0x10) >> 0xF)) + (s32) var_a0);
            var_s0 += temp_v1;
        }
        *var_a1 = var_v0;
        var_s2 += 1;
        var_a1 += 1;
    } while (var_s2 < 3);
    var_s2 = 0;
    temp_v1 = (arg1->packing_flags >> 2) + 1;
    if ((s32)arg1->last_bone_index >= var_s2) {
        temp_s6 = arg2;
        temp_s5 = temp_v1 * 4;
        var_s1 = var_s3 + 2;
        do {
            var_v1 = var_s0 + 1;
            if (((RenderAnimByteChannel *)var_s0)->constant_marker != 0) {
                var_v0_2 = ((RenderAnimByteChannel *)var_s0)->value_or_samples[0];
                var_s0 += 4;
            } else {
                var_v0_2 = *(u8 *)(var_v1 + temp_s6);
                var_s0 += temp_s5;
            }
            *var_s3 = var_v0_2 * 0x10;
            var_v1 = var_s0 + 1;
            if (((RenderAnimByteChannel *)var_s0)->constant_marker != 0) {
                var_v0_3 = ((RenderAnimByteChannel *)var_s0)->value_or_samples[0];
                var_s0 += 4;
            } else {
                var_v0_3 = *(u8 *)(var_v1 + temp_s6);
                var_s0 += temp_s5;
            }
            var_s1[-1] = var_v0_3 * 0x10;
            var_v1 = var_s0 + 1;
            if (((RenderAnimByteChannel *)var_s0)->constant_marker != 0) {
                var_v0_4 = ((RenderAnimByteChannel *)var_s0)->value_or_samples[0];
                var_s0 += 4;
            } else {
                var_v0_4 = *(u8 *)(var_v1 + temp_s6);
                var_s0 += temp_s5;
            }
            var_s1[0] = var_v0_4 * 0x10;
            __asm__ volatile("");
            RotMatrixYXZ(var_s3, var_s4);
            var_s4 += 0x20;
            var_s1 += 4;
            nlim = arg1->last_bone_index;
            __asm__ volatile("");
            var_s2 += 1;
            var_s3 += 4;
        } while (nlim >= var_s2);
    }
}
