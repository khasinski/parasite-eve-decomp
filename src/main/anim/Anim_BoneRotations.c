#include "common.h"
#include "pe1/render_object.h"

#define UH(ptr, off) (*(u16 *)((char *)(ptr) + (off)))
#define UB(ptr, off) (*(u8 *)((char *)(ptr) + (off)))
#define W(ptr, off) (*(s32 *)((char *)(ptr) + (off)))

#define APPLY_COMPONENT(entry, count, scratch, dst_off, value_off, abs_bit, add_bit) \
    do { \
        v1 = UB((entry), 0xA7); \
        v0 = v1 & (abs_bit); \
        if (v0 != 0) { \
            v0 = (count) << 3; \
            v1 = UH((entry), (value_off)); \
            v0p = (char *)(v0 + (s32)(scratch)); \
            *(u16 *)(v0p + (dst_off)) = v1; \
        } else { \
            v0 = v1 & (add_bit); \
            if (v0 != 0) { \
                v0 = (count) << 3; \
                v0p = (char *)(v0 + (s32)(scratch)); \
                v1 = *(u16 *)(v0p + (dst_off)); \
                a0tmp = UH((entry), (value_off)); \
                v1 += a0tmp; \
                *(u16 *)(v0p + (dst_off)) = v1; \
            } \
        } \
    } while (0)

void Anim_BuildRotationMatrices(char *obj, RenderAnimationDataHeader *data, int arg2) {
    char *s1 = obj;
    register char *scratch asm("$18") = (char *)0x1F800000;
    register char *s0 asm("$16");
    register int count asm("$5");
    int v1;
    register int v0 asm("$2");
    register char *v0p asm("$2");
    int a0tmp;
    char *rot;
    char *mat_base;
    int mode;

    if (data == 0) {
        return;
    }

    ((RenderObjectEntity *)s1)->hit_body.radius = data->object_value7c;
    ((RenderObjectEntity *)s1)->hit_body.value0 = data->object_value74;
    ((RenderObjectEntity *)s1)->hit_body.value1 = data->object_value76;
    ((RenderObjectEntity *)s1)->hit_body.value2 = data->object_value78;

    mode = data->encoding_flags & 3;
    if (mode == 2) {
        Anim_DecodeBoneRotationsShort(s1, data, (s16)arg2);
    } else {
        Anim_DecodeBoneRotationsByte(s1, data, (s16)arg2);
    }

    s0 = s1;
    do {
        count = UB(s0, 0xA6);
        if (count > 0) {
            APPLY_COMPONENT(s0, count, scratch, 0, 0xA0, 0x08, 0x01);
            APPLY_COMPONENT(s0, count, scratch, 2, 0xA2, 0x10, 0x02);
            APPLY_COMPONENT(s0, count, scratch, 4, 0xA4, 0x20, 0x04);
            rot = (char *)((u32)scratch | (count << 3));
            mat_base = (char *)((RenderObjectEntity *)s1)->active_matrix;
            RotMatrixYXZ((GteShortVector *)rot, (GteMatrix *)(mat_base + (count << 5)));
        }
        s0 += 8;
    } while ((s32)s0 < (s32)(s1 + 0x10));
}

/* Bone rotation keyframe decoders for the byte- and halfword-packed
 * animation formats. */

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
            var_v0 = *(s16 *)((((arg2 << 0x10) >> 0xF)) + (s32) var_a0);
            var_s0 += temp_v1;
        }
        *var_a1 = var_v0;
        var_s2 += 1;
        var_a1 += 1;
    } while (var_s2 < 3);
    var_s2 = 0;
    temp_v1 = (arg1->packing_flags >> 2) + 1;
    if (arg1->last_bone_index >= var_s2) {
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
            RotMatrixYXZ((GteShortVector *)var_s3, (GteMatrix *)var_s4);
            var_s4 += 0x20;
            var_s1 += 4;
            nlim = arg1->last_bone_index;
            __asm__ volatile("");
            var_s2 += 1;
            var_s3 += 4;
        } while (nlim >= var_s2);
    }
}

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
            var_v0 = *(s16 *)((((arg2 << 0x10) >> 0xF)) + (s32) var_v1);
            var_s0 += temp_a3 * 4;
        }
        *var_a0 = var_v0;
        temp_v0 = var_s3 + 1;
        var_s3 = temp_v0;
        var_a0 += 1;
    } while (temp_v0 < 3);
    var_s3 = 0;
    if (arg1->last_bone_index >= var_s3) {
        temp_s6 = ((arg2 << 0x10) >> 0xF);
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
            RotMatrixYXZ((GteShortVector *)var_s2, (GteMatrix *)var_s4);
            var_s4 += 0x20;
            var_s1 += 4;
            temp_v0_2 = var_s3 + 1;
            var_s3 = temp_v0_2;
            var_s2 += 4;
            nlim = arg1->last_bone_index;
        } while (nlim >= var_s3);
    }
}
