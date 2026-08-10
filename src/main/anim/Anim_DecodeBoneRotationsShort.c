#include "common.h"
#include "pe1/render_object.h"
void RotMatrixYXZ();

void Anim_DecodeBoneRotationsShort(RenderObjectEntity *arg0, RenderAnimationDataHeader *arg1, s16 arg2) {
    register u16 *var_s2 asm("$18");
    s16 var_s3;
    s16 temp_v0;
    s16 temp_v0_2;
    register s32 nlim asm("$3");
    s32 var_v0;
    s32 *var_a0;
    s32 temp_a3;
    s32 temp_s5;
    s32 temp_s6;
    char *var_s4;
    u16 var_v0_2;
    u16 var_v0_3;
    u16 var_v0_4;
    char *var_s0;
    u16 *var_s1;
    char *var_v1;

    var_s2 = (u16 *)0x1F800000;
    var_s0 = (char *)(arg1 + 1);
    var_s3 = 0;
    var_s4 = (char *)arg0->matrices;
    arg0->active_matrix = (RenderMatrix *)var_s4;
    var_a0 = (s32 *) (var_s4 + 0x14);
    temp_a3 = (arg1->packing_flags >> 1) + 1;
    do {
        var_v1 = var_s0 + 2;
        if (((RenderAnimShortChannel *)var_s0)->constant_marker != 0) {
            var_v0 = ((RenderAnimShortChannel *)var_s0)->samples.signed_values[0];
            var_s0 += 4;
        } else {
            var_v0 = *(s16 *)((((s32) (arg2 << 0x10) >> 0xF)) + (s32) var_v1);
            var_s0 += temp_a3 * 4;
        }
        *var_a0 = (s32) var_v0;
        temp_v0 = var_s3 + 1;
        var_s3 = temp_v0;
        var_a0 += 1;
    } while (temp_v0 < 3);
    var_s3 = 0;
    if ((s32)arg1->last_bone_index >= var_s3) {
        temp_s6 = ((s32) (arg2 << 0x10) >> 0xF);
        temp_s5 = temp_a3 * 4;
        var_s1 = var_s2 + 2;
        do {
            var_v1 = var_s0 + 2;
            if (((RenderAnimShortChannel *)var_s0)->constant_marker != 0) {
                var_v0_2 = ((RenderAnimShortChannel *)var_s0)->samples.unsigned_values[0];
                var_s0 = var_s0 + 4;
            } else {
                var_v0_2 = *(u16 *)(temp_s6 + (s32) var_v1);
                var_s0 = var_s0 + temp_s5;
            }
            *var_s2 = var_v0_2;
            var_v1 = var_s0 + 2;
            if (((RenderAnimShortChannel *)var_s0)->constant_marker != 0) {
                var_v0_3 = ((RenderAnimShortChannel *)var_s0)->samples.unsigned_values[0];
                var_s0 = var_s0 + 4;
            } else {
                var_v0_3 = *(u16 *)(temp_s6 + (s32) var_v1);
                var_s0 = var_s0 + temp_s5;
            }
            var_s1[-1] = var_v0_3;
            var_v1 = var_s0 + 2;
            if (((RenderAnimShortChannel *)var_s0)->constant_marker != 0) {
                var_v0_4 = ((RenderAnimShortChannel *)var_s0)->samples.unsigned_values[0];
                var_s0 = var_s0 + 4;
            } else {
                var_v0_4 = *(u16 *)(temp_s6 + (s32) var_v1);
                var_s0 = var_s0 + temp_s5;
            }
            var_s1[0] = var_v0_4;
            __asm__ volatile("");
            RotMatrixYXZ(var_s2, var_s4);
            var_s4 += 0x20;
            var_s1 += 4;
            temp_v0_2 = var_s3 + 1;
            var_s3 = temp_v0_2;
            var_s2 += 4;
            nlim = arg1->last_bone_index;
        } while (nlim >= var_s3);
    }
}
