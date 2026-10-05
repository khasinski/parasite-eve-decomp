/* Initial full-function candidate; not integrated and not yet matching. */
#include "scene_e19_recovered.h"
#include "pe1/gte.h"
#include "m2c_macros.h"
#define NULL ((void *)0)
extern u16 D_800942EC;
extern void **D_8009D254;
extern void *D_800B0E64;
extern u16 D_800E11EA;
extern M2C_UNK D_800E11FA;
extern u16 D_800E120A;
extern void *D_800E2368;
extern s16 D_800F336E;
extern u16 D_800F3370;
extern s16 D_800F3372;
extern u16 D_800F3376;
extern u16 D_800F3378;
extern M2C_UNK D_80192E08;
extern s32 D_8019B668;
extern void *D_8019B680;
extern void *D_8019B684;
extern void *D_8019B688;
extern void *D_8019B68C;
extern void *D_8019B690;

s32 func_80192F9C(s32 mode, SceneE19RecoveredState *effect) {
    GteShortVector sp30;
    GteShortVector sp38;                            /* compiler-managed */
    GteRotation sp40;
    GteShortVector sp48;                            /* compiler-managed */
    RenderColor sp50;
    RenderColor sp58;
    GteShortVector sp60;
    GteShortVector sp68;
    GteShortVector sp70;
    GteShortVector sp78;
    GteMatrix sp80;
    GteVector spA0;
    GteShortVector spB0;
    GteShortVector spB8;
    GteMatrix spC0;
    GteVector spE0;
    GteMatrix spF0;
    GteVector sp110;
    GteMatrix sp120;
    GteVector sp140;
    GteMatrix sp150;
    GteVector sp170;
    GteMatrix sp180;
    GteVector sp1A0;
    GteMatrix sp1B0;
    GteVector sp1D0;
    GteMatrix sp1E0;
    GteVector sp200;
    GteMatrix sp210;
    GteVector sp230;
    GteMatrix sp240;
    GteVector sp260;
    GteMatrix sp270;
    GteVector sp290;
    GteMatrix *var_a0;
    GteMatrix *var_s0_2;
    GteShortVector *temp_s2_2;
    GteVector *var_a1_2;
    SceneE19RecoveredObject *temp_a0;
    SceneE19RecoveredObject *temp_a0_2;
    s16 temp_a2;
    s16 temp_s7;
    s16 temp_s7_2;
    s16 temp_v0_28;
    s16 temp_v0_29;
    s16 temp_v0_30;
    s16 temp_v1;
    s16 temp_v1_3;
    s16 temp_v1_4;
    s16 var_s3_6;
    s16 var_v0;
    s32 temp_hi;
    s32 temp_s0;
    s32 temp_s0_10;
    s32 temp_s0_11;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s0_4;
    s32 temp_s0_5;
    s32 temp_s0_6;
    s32 temp_s0_7;
    s32 temp_s0_8;
    s32 temp_s0_9;
    s32 temp_s1;
    s32 temp_s1_2;
    s32 temp_s2;
    s32 temp_s2_10;
    s32 temp_s2_11;
    s32 temp_s2_3;
    s32 temp_s2_4;
    s32 temp_s2_5;
    s32 temp_s2_6;
    s32 temp_s2_7;
    s32 temp_s2_8;
    s32 temp_s2_9;
    s32 temp_s4;
    s32 temp_s4_10;
    s32 temp_s4_11;
    s32 temp_s4_12;
    s32 temp_s4_2;
    s32 temp_s4_3;
    s32 temp_s4_4;
    s32 temp_s4_5;
    s32 temp_s4_6;
    s32 temp_s4_7;
    s32 temp_s4_8;
    s32 temp_s4_9;
    s32 temp_s6;
    s32 temp_s6_2;
    s32 temp_s6_3;
    s32 temp_s6_4;
    s32 temp_v0_10;
    s32 temp_v0_11;
    s32 temp_v0_12;
    s32 temp_v0_13;
    s32 temp_v0_14;
    s32 temp_v0_15;
    s32 temp_v0_19;
    s32 temp_v0_20;
    s32 temp_v0_21;
    s32 temp_v0_22;
    s32 temp_v0_23;
    s32 temp_v0_24;
    s32 temp_v0_25;
    s32 temp_v0_26;
    s32 temp_v0_27;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 temp_v0_9;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 var_a1;
    s32 var_a2_2;
    s32 var_s0;
    s32 var_s3;
    s32 var_s3_10;
    s32 var_s3_11;
    s32 var_s3_12;
    s32 var_s3_13;
    s32 var_s3_14;
    s32 var_s3_15;
    s32 var_s3_16;
    s32 var_s3_2;
    s32 var_s3_3;
    s32 var_s3_4;
    s32 var_s3_5;
    s32 var_s3_7;
    s32 var_s3_8;
    s32 var_s3_9;
    s32 var_s4;
    s32 var_s4_3;
    s32 var_v1;
    s32 var_v1_10;
    s32 var_v1_11;
    s32 var_v1_12;
    s32 var_v1_13;
    s32 var_v1_14;
    s32 var_v1_3;
    s32 var_v1_4;
    s32 var_v1_5;
    s32 var_v1_6;
    s32 var_v1_7;
    s32 var_v1_8;
    s32 var_v1_9;
    u16 temp_v0_16;
    u16 temp_v1_2;
    u16 var_a2;
    u16 var_a2_10;
    u16 var_a2_11;
    u16 var_a2_3;
    u16 var_a2_4;
    u16 var_a2_5;
    u16 var_a2_6;
    u16 var_a2_7;
    u16 var_a2_8;
    u16 var_a2_9;
    u16 var_v1_2;
    u32 temp_v0_17;
    u32 temp_v0_18;
    u32 var_s4_2;
    void *temp_v0;
    void *temp_v0_2;
    void *temp_v0_31;
    void *temp_v0_32;
    void *temp_v0_3;
    void *temp_v0_4;
    void *temp_v0_5;
    void *temp_v1_5;

    sp40 = *(GteRotation *)&D_8018F1D4;
    sp50 = D_8018F1DC;
    sp58 = D_8018F1E0;
    if (mode != 1) {
        if (mode < 2) {
            if (mode != 0) {
                return 0;
            }
            temp_v0 = func_8006E498(D_800B0E64, 0xC54C0704U);
            D_8019B680 = temp_v0;
            func_800C6D5C(temp_v0, 0U, 0U);
            temp_v0_2 = func_8006E498(D_800B0E64, 0xC58C0704U);
            D_8019B684 = temp_v0_2;
            func_800C6D5C(temp_v0_2, 0U, 0U);
            temp_v0_3 = func_8006E498(D_800B0E64, 0xC5CC0704U);
            D_8019B688 = temp_v0_3;
            func_800C6D5C(temp_v0_3, 0U, 0U);
            temp_v0_4 = func_8006E498(D_800B0E64, 0xC60C0704U);
            D_8019B68C = temp_v0_4;
            func_800C6D5C(temp_v0_4, 0U, 0U);
            temp_v0_5 = func_8006E498(D_800B0E64, 0xC64C0704U);
            D_8019B690 = temp_v0_5;
            func_800C6D5C(temp_v0_5, 0U, 0U);
            effect->state = 0;
            effect->timer = 0;
            func_800CE870((s8 *) D_800F32D0->pool, 1, &effect->position.x);
            effect->position.y = (s16) D_800942EC;
            func_800CE870((s8 *) D_800F32D0->pool, 0, &effect->endpoint.x);
            func_800CE9D4((struct RoomFxTransformOwner *) D_800F32D0->pool, 0, &effect->origin);
            return func_800CE560(D_800F33E0->pool, 0xC, 8, &D_80192E08);
        }
        if (mode != 2) {
            return 0;
        }
        sp30.x = (u16) effect->position.x;
        sp30.y = (s16) (u16) effect->position.y;
        sp30.z = (s16) (u16) effect->position.z;
        gte_ctc2_0((u32) M2C_FIELD(D_800BCFA4.value, s32 *, 0));
        gte_ctc2_1(M2C_FIELD(D_800BCFA4.value, u32 *, 4));
        gte_ctc2_2(M2C_FIELD(D_800BCFA4.value, u32 *, 8));
        gte_ctc2_3(M2C_FIELD(D_800BCFA4.value, u32 *, 0xC));
        gte_ctc2_4(M2C_FIELD(D_800BCFA4.value, u32 *, 0x10));
        gte_ctc2_5(M2C_FIELD(D_800BCFA4.value, u32 *, 0x14));
        gte_ctc2_6(M2C_FIELD(D_800BCFA4.value, u32 *, 0x18));
        gte_ctc2_7(M2C_FIELD(D_800BCFA4.value, u32 *, 0x1C));
        D_800F3372 = 0;
        D_800F3374 = 4;
        temp_v1 = effect->state;
        switch (temp_v1) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
            temp_v0_6 = func_80077CF4(effect->timer << 6);
            var_s3 = temp_v0_6 >> 5;
            if (temp_v0_6 < 0) {
                var_s3 = (s32) (temp_v0_6 + 0x1F) >> 5;
            }
            D_800F3368.parameter00 = 0x40;
            D_800F336A = 4;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3370 = D_800E2850[M2C_FIELD(&D_800E11FA, u16 *, 0)];
            if (D_800E27EC & 1) {
                var_s3 = (var_s3 * 2) / 3;
            }
            sp68.pad = 1;
            sp68.x = 0x400;
            sp68.y = 0;
            sp68.z = 0;
            sp60.x = (u16) sp30.x;
            sp60.z = (s16) (u16) sp30.z;
            sp60.y = (s16) D_800942EC;
            func_800CEE20(&sp60, (GteRotation *) &sp68, 0x2000, 0x2000, 0, func_80077AA4(0, D_800E120A + 2) & 0xFFFF, 1, var_s3, NULL);
            break;
        case 1:                                     /* switch 2 */
            var_s3_2 = 0x80;
            D_800F336A = 4;
            D_800F3368.parameter00 = 0x40;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3370 = D_800E2850[M2C_FIELD(&D_800E11FA, u16 *, 0)];
            temp_s6 = effect->timer << 6;
            if (D_800E27EC & 1) {
                var_s3_2 = 0x55;
            }
            sp78.pad = 1;
            sp78.x = 0x400;
            sp78.y = 0;
            sp78.z = 0;
            sp70.x = (u16) sp30.x;
            sp70.z = (s16) (u16) sp30.z;
            sp70.y = (s16) D_800942EC;
            func_800CEE20(&sp70, (GteRotation *) &sp78, 0x2000, 0x2000, 0, func_80077AA4(0, D_800E120A + 2) & 0xFFFF, 1, var_s3_2, NULL);
            var_v1 = func_80077CF4(temp_s6);
            if (var_v1 < 0) {
                var_v1 += 0x1F;
            }
            var_s3_3 = var_v1 >> 5;
            if (D_800E27EC & 1) {
                var_s3_3 = (var_s3_3 * 2) / 3;
            }
            var_s4 = func_80077CF4(temp_s6);
            if (D_800E27EC & 1) {
                temp_v0_7 = var_s4 * 0xF;
                var_s4 = temp_v0_7 >> 4;
                if (temp_v0_7 < 0) {
                    var_s4 = (s32) (temp_v0_7 + 0xF) >> 4;
                }
            }
            sp48.x = -0x400;
            sp48.y = 0;
            sp48.z = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2 = D_800E1204[D_800F336C];
            temp_s0 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2 += 4;
            }
            func_800C6EC0(temp_s0, func_80077AA4(0x20, (s32) var_a2) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754(&sp48, &sp80);
            temp_v0_8 = (s32) (var_s4 + ((u32) var_s4 >> 0x1F)) >> 1;
            spA0.x = temp_v0_8;
            spA0.y = temp_v0_8;
            sp80.t[0] = (s32) (s16) sp30.x;
            sp80.t[1] = (s32) sp30.y;
            sp80.t[2] = (s32) sp30.z;
            spA0.z = (effect->timer << 6) + 0x400;
            func_80078CC4(&sp80, &spA0);
            func_800C6EF8(D_8019B684);
            func_800C6FA0(D_8019B684, 0x40U);
            func_800C71E4(D_8019B684, &sp80);
            func_800C6F4C(D_8019B684);
            gte_ctc2_0((u32) M2C_FIELD(D_800BCFA4.value, s32 *, 0));
            gte_ctc2_1(M2C_FIELD(D_800BCFA4.value, u32 *, 4));
            gte_ctc2_2(M2C_FIELD(D_800BCFA4.value, u32 *, 8));
            gte_ctc2_3(M2C_FIELD(D_800BCFA4.value, u32 *, 0xC));
            gte_ctc2_4(M2C_FIELD(D_800BCFA4.value, u32 *, 0x10));
            gte_ctc2_5(M2C_FIELD(D_800BCFA4.value, u32 *, 0x14));
            gte_ctc2_6(M2C_FIELD(D_800BCFA4.value, u32 *, 0x18));
            gte_ctc2_7(M2C_FIELD(D_800BCFA4.value, u32 *, 0x1C));
            func_800D004C(&sp30, 0x12C, 0x12C, 0xC, NULL, 0x1000, 0x1000, &sp50, NULL, var_s3_3, 1);
            temp_v0_9 = func_80077DC4(temp_s6);
            var_s3_4 = temp_v0_9 >> 5;
            if (temp_v0_9 < 0) {
                var_s3_4 = (s32) (temp_v0_9 + 0x1F) >> 5;
            }
            temp_v0_10 = func_80077CF4(temp_s6);
            func_800D0728(&sp30, 0x7D0, 0xB54, 0x18, &sp40, temp_v0_10, temp_v0_10, NULL, &sp50, var_s3_4, 1);
            break;
        case 2:                                     /* switch 2 */
            var_s3_5 = 0x80;
            D_800F3368.parameter00 = 0x40;
            D_800F336A = 4;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F3370 = D_800E2850[M2C_FIELD(&D_800E11FA, u16 *, 0)];
            D_800F336C = 3;
            D_800F336E = 1;
            temp_s6_2 = effect->timer << 6;
            if (D_800E27EC & 1) {
                var_s3_5 = 0x55;
            }
            spB8.pad = 1;
            spB8.x = 0x400;
            var_s4_2 = 0x1000;
            spB8.y = 0;
            spB8.z = 0;
            spB0.x = (u16) sp30.x;
            spB0.z = (s16) (u16) sp30.z;
            spB0.y = (s16) D_800942EC;
            func_800CEE20(&spB0, (GteRotation *) &spB8, 0x2000, 0x2000, 0, func_80077AA4(0, D_800E120A + 2) & 0xFFFF, 1, var_s3_5, NULL);
            if (D_800E27EC & 1) {
                var_s4_2 = 0xF00;
            }
            sp48.x = -0x400;
            sp48.y = 0;
            sp48.z = D_800E27EC << 5;
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = D_800E2850[M2C_FIELD(&D_800E11FA, u16 *, -0x10)];
            var_v1_2 = D_800E1204[D_800F3368.palette];
            temp_s0_2 = (D_800E2850[M2C_FIELD(&D_800E11FA, u16 *, -0x10)] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F3368.palette == 4) && (D_800F3428 != 0)) {
                var_v1_2 += 4;
            }
            func_800C6EC0(temp_s0_2, func_80077AA4(0x20, (s32) var_v1_2) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &spC0);
            temp_v0_11 = (s32) (var_s4_2 + (var_s4_2 >> 0x1F)) >> 1;
            spE0.x = temp_v0_11;
            spE0.y = temp_v0_11;
            spC0.t[0] = (s32) (s16) sp30.x;
            spC0.t[1] = (s32) sp30.y;
            spC0.t[2] = (s32) sp30.z;
            spE0.z = (effect->timer << 6) + 0x800;
            func_80078CC4(&spC0, &spE0);
            func_800C6EF8(D_8019B684);
            func_800C6FA0(D_8019B684, 0x40U);
            func_800C71E4(D_8019B684, &spC0);
            func_800C6F4C(D_8019B684);
            gte_ctc2_0((u32) M2C_FIELD(D_800BCFA4.value, s32 *, 0));
            gte_ctc2_1(M2C_FIELD(D_800BCFA4.value, u32 *, 4));
            gte_ctc2_2(M2C_FIELD(D_800BCFA4.value, u32 *, 8));
            gte_ctc2_3(M2C_FIELD(D_800BCFA4.value, u32 *, 0xC));
            gte_ctc2_4(M2C_FIELD(D_800BCFA4.value, u32 *, 0x10));
            gte_ctc2_5(M2C_FIELD(D_800BCFA4.value, u32 *, 0x14));
            gte_ctc2_6(M2C_FIELD(D_800BCFA4.value, u32 *, 0x18));
            gte_ctc2_7(M2C_FIELD(D_800BCFA4.value, u32 *, 0x1C));
            func_800D004C(&sp30, 0x12C, 0x12C, 0xC, NULL, 0x1000, 0x1000, &sp50, NULL, var_s3_5, 1);
            var_s3_6 = 0x80;
            if (D_800E27EC & 1) {
                var_s3_6 = 0x64;
            }
            temp_v0_12 = func_80077DC4(temp_s6_2);
            var_s4_3 = temp_v0_12 >> 3;
            if (temp_v0_12 < 0) {
                var_s4_3 = (s32) (temp_v0_12 + 7) >> 3;
            }
            temp_v0_13 = func_80077CF4(temp_s6_2);
            temp_hi = (temp_v0_13 * 2) / 3;
            var_s0 = 0;
            temp_v0_14 = (s32) (temp_v0_13 * 2) >> 0x1F;
            sp48.x = 0x400;
            sp48.y = 0;
            sp48.z = 0;
            var_v1_3 = 0 << 8;
            do {
                temp_s2 = var_v1_3 + (D_800E27EC * 4);
                sp38.z = (s16) (u16) sp30.z;
                sp38.x = (u16) sp30.x;
                sp38.y = (s16) (u16) sp30.y;
                var_v1_4 = func_80077DC4(temp_s2) * 0x7D0;
                if (var_v1_4 < 0) {
                    var_v1_4 += 0xFFF;
                }
                sp38.x = (u16) sp38.x + (var_v1_4 >> 0xC);
                var_v1_5 = func_80077CF4(temp_s2) * 0x7D0;
                if (var_v1_5 < 0) {
                    var_v1_5 += 0xFFF;
                }
                var_s0 += 1;
                sp48.z = temp_s2 + 0x400;
                sp38.z = (u16) sp38.z + (var_v1_5 >> 0xC);
                func_800D0E88((GteShortVector *) &sp38, (GteRotation *) &sp48, temp_hi - temp_v0_14, var_s4_3, &sp50, NULL, NULL, (s32) var_s3_6, 1);
                var_v1_3 = var_s0 << 8;
            } while (var_s0 < 0x10);
            temp_v0_15 = func_80077DC4(temp_s6_2);
            var_s3_7 = temp_v0_15 >> 5;
            if (temp_v0_15 < 0) {
                var_s3_7 = (s32) (temp_v0_15 + 0x1F) >> 5;
            }
            temp_s4 = func_80077DC4(temp_s6_2);
            temp_s2_2 = &effect->endpoint;
            temp_v1_2 = (u16) effect->origin.x;
            sp48.x = temp_v1_2;
            temp_v0_16 = (u16) effect->origin.y;
            sp48.y = temp_v0_16;
            sp48.x = temp_v1_2 - 0x200;
            sp48.y = temp_v0_16 + 0x400;
            sp48.z = (u16) effect->origin.z;
            func_800D0728(temp_s2_2, 0x7D0, 0xA8C, 0x20, (GteRotation *) &sp48, temp_s4, temp_s4, &sp58, NULL, var_s3_7, 1);
            sp48.x += 0x400;
            func_800D0728(temp_s2_2, 0x7D0, 0xA8C, 0x20, (GteRotation *) &sp48, temp_s4, temp_s4, &sp58, NULL, var_s3_7, 1);
            break;
        case 3:                                     /* switch 2 */
            temp_v1_3 = effect->timer;
            temp_s6_3 = temp_v1_3 << 5;
            if (temp_v1_3 == 0) {
                var_a1 = 0x46;
                var_a2_2 = 2;
                goto block_84;
            }
            if (temp_v1_3 < 9) {
                var_a1 = 0x80 - (temp_v1_3 * 0x10);
                var_a2_2 = 1;
block_84:
                func_800D1AE0(&sp50, var_a1, var_a2_2, 8);
            }
            temp_v0_17 = func_80077DC4(temp_s6_3);
            temp_s4_2 = ((s32) ((temp_v0_17 >> 0x1F) + temp_v0_17) >> 1) + 0x400;
            temp_s2_3 = (func_80077CF4(temp_s6_3) / 6) + 0x555;
            var_s3_8 = 0x80;
            if (D_800E27EC & 1) {
                var_s3_8 = 0x78;
            }
            sp48.x = 0;
            sp48.y = D_800E27EC << 5;
            sp48.z = 0;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_3 = D_800E1204[D_800F336C];
            temp_s0_3 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_3 += 4;
            }
            func_800C6EC0(temp_s0_3, func_80077AA4(0x20, (s32) var_a2_3) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &spF0);
            sp110.x = temp_s4_2;
            sp110.y = temp_s2_3;
            sp110.z = temp_s4_2;
            spF0.t[0] = (s32) (s16) sp30.x;
            spF0.t[1] = (s32) sp30.y;
            spF0.t[2] = (s32) sp30.z;
            func_80078CC4(&spF0, &sp110);
            func_800C6EF8(D_8019B680);
            func_800C6FA0(D_8019B680, var_s3_8 & 0xFFFF);
            func_800C71E4(D_8019B680, &spF0);
            func_800C6F4C(D_8019B680);
            temp_s4_3 = (func_80077CF4(temp_s6_3) / 3) + 0x400;
            temp_v0_18 = func_80077DC4(temp_s6_3);
            temp_s2_4 = (s32) ((temp_v0_18 >> 0x1F) + temp_v0_18) >> 1;
            var_v1_6 = func_80077DC4(temp_s6_3);
            if (var_v1_6 < 0) {
                var_v1_6 += 0x1F;
            }
            var_s3_9 = var_v1_6 >> 5;
            if (D_800E27EC & 1) {
                temp_v0_19 = var_s3_9 * 0xF;
                var_s3_9 = temp_v0_19 >> 4;
                if (temp_v0_19 < 0) {
                    var_s3_9 = (s32) (temp_v0_19 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC * -0x30;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_4 = D_800E1204[D_800F336C];
            temp_s0_4 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_4 += 4;
            }
            func_800C6EC0(temp_s0_4, func_80077AA4(0x20, (s32) var_a2_4) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp120);
            sp140.x = temp_s4_3;
            sp140.y = temp_s2_4;
            sp140.z = temp_s4_3;
            sp120.t[0] = (s32) (s16) sp30.x;
            sp120.t[1] = (s32) sp30.y;
            sp120.t[2] = (s32) sp30.z;
            func_80078CC4(&sp120, &sp140);
            func_800C6EF8(D_8019B688);
            func_800C6FA0(D_8019B688, (var_s3_9 / 2) & 0xFFFF);
            func_800C71E4(D_8019B688, &sp120);
            func_800C6F4C(D_8019B688);
            temp_s1 = (effect->timer << 0xA) / 56;
            temp_s4_4 = (func_80077CF4(temp_s1) / 4) + 0xC00;
            temp_s2_5 = (func_80077DC4(temp_s1) / 4) + 0x400;
            var_v1_7 = func_80077DC4(temp_s1);
            if (var_v1_7 < 0) {
                var_v1_7 += 0x1F;
            }
            var_s3_10 = var_v1_7 >> 5;
            if (D_800E27EC & 1) {
                temp_v0_20 = var_s3_10 * 0xF;
                var_s3_10 = temp_v0_20 >> 4;
                if (temp_v0_20 < 0) {
                    var_s3_10 = (s32) (temp_v0_20 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC * -0x20;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_5 = D_800E1204[D_800F336C];
            temp_s0_5 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_5 += 4;
            }
            func_800C6EC0(temp_s0_5, func_80077AA4(0x20, (s32) var_a2_5) & 0xFFFF);
            func_800C6ED8(1);
            temp_s7 = sp30.y;
            sp30.y = temp_s7 - (effect->timer * 0x18);
            func_80079754((GteShortVector *) &sp48, &sp150);
            sp170.x = temp_s4_4;
            sp170.y = temp_s2_5;
            sp170.z = temp_s4_4;
            sp150.t[0] = (s32) (s16) sp30.x;
            sp150.t[1] = (s32) sp30.y;
            sp150.t[2] = (s32) sp30.z;
            func_80078CC4(&sp150, &sp170);
            func_800C6EF8(D_8019B68C);
            func_800C6FA0(D_8019B68C, (var_s3_10 / 2) & 0xFFFF);
            func_800C71E4(D_8019B68C, &sp150);
            func_800C6F4C(D_8019B68C);
            sp30.y = temp_s7;
            gte_ctc2_0((u32) M2C_FIELD(D_800BCFA4.value, s32 *, 0));
            gte_ctc2_1(M2C_FIELD(D_800BCFA4.value, u32 *, 4));
            gte_ctc2_2(M2C_FIELD(D_800BCFA4.value, u32 *, 8));
            gte_ctc2_3(M2C_FIELD(D_800BCFA4.value, u32 *, 0xC));
            gte_ctc2_4(M2C_FIELD(D_800BCFA4.value, u32 *, 0x10));
            gte_ctc2_5(M2C_FIELD(D_800BCFA4.value, u32 *, 0x14));
            gte_ctc2_6(M2C_FIELD(D_800BCFA4.value, u32 *, 0x18));
            gte_ctc2_7(M2C_FIELD(D_800BCFA4.value, u32 *, 0x1C));
            func_800D004C(&sp30, 0x9C4, 0x9C4, 0xC, NULL, 0x1000, 0x1000, &sp50, NULL, var_s3_10, 1);
            temp_s4_5 = (func_80077CF4(temp_s6_3) / 4) + 0xC00;
            temp_v0_21 = func_80077DC4(temp_s6_3);
            var_s3_11 = temp_v0_21 >> 5;
            if (temp_v0_21 < 0) {
                var_s3_11 = (s32) (temp_v0_21 + 0x1F) >> 5;
            }
            func_800D0728(&sp30, 0x76C, 0xA28, 0x18, &sp40, temp_s4_5, temp_s4_5, &sp58, NULL, var_s3_11, 1);
            sp30.y = (u16) sp30.y - 0x400;
            temp_s4_6 = ((s32) (func_80077CF4(temp_s6_3) * 3) / 2) + 0x1000;
            func_800D0728(&sp30, 0x3E8, 0x5DC, 0x18, &sp40, temp_s4_6, temp_s4_6, &sp50, NULL, var_s3_11, 1);
            sp30.y = (u16) sp30.y + 0x400;
            temp_s4_7 = (func_80077CF4(temp_s1) / 4) + 0x1200;
            temp_s2_6 = func_80077DC4(temp_s1);
            var_v1_8 = func_80077DC4(temp_s1);
            if (var_v1_8 < 0) {
                var_v1_8 += 0x1F;
            }
            var_s3_12 = var_v1_8 >> 5;
            if (D_800E27EC & 1) {
                temp_v0_22 = var_s3_12 * 0xF;
                var_s3_12 = temp_v0_22 >> 4;
                if (temp_v0_22 < 0) {
                    var_s3_12 = (s32) (temp_v0_22 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = (D_800E27EC << 5) + 0x400;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_6 = D_800E1204[D_800F336C];
            temp_s0_6 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_6 += 4;
            }
            func_800C6EC0(temp_s0_6, func_80077AA4(0x60, (s32) var_a2_6) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp180);
            sp1A0.x = temp_s4_7;
            sp1A0.y = temp_s2_6;
            sp1A0.z = temp_s4_7;
            sp180.t[0] = (s32) (s16) sp30.x;
            sp180.t[1] = (s32) sp30.y;
            sp180.t[2] = (s32) sp30.z;
            func_80078CC4(&sp180, &sp1A0);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, var_s3_12 & 0xFFFF);
            func_800C71E4(D_8019B690, &sp180);
            func_800C6F4C(D_8019B690);
            temp_s4_8 = (func_80077CF4(temp_s1) / 6) + 0x1200;
            temp_s2_7 = func_80077DC4(temp_s1) * 2;
            var_v1_9 = func_80077DC4(temp_s1);
            if (var_v1_9 < 0) {
                var_v1_9 += 0x1F;
            }
            var_s3_13 = var_v1_9 >> 5;
            if (!(D_800E27EC & 1)) {
                temp_v0_23 = var_s3_13 * 0xF;
                var_s3_13 = temp_v0_23 >> 4;
                if (temp_v0_23 < 0) {
                    var_s3_13 = (s32) (temp_v0_23 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_7 = D_800E1204[D_800F336C];
            temp_s0_7 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_7 += 4;
            }
            func_800C6EC0(temp_s0_7, func_80077AA4(0x60, (s32) var_a2_7) & 0xFFFF);
            func_800C6ED8(1);
            var_s0_2 = &sp1B0;
            func_80079754((GteShortVector *) &sp48, var_s0_2);
            var_a0 = var_s0_2;
            var_a1_2 = &sp1D0;
            sp1D0.x = temp_s4_8;
            sp1D0.y = temp_s2_7;
            sp1D0.z = temp_s4_8;
            sp1B0.t[0] = (s32) (s16) sp30.x;
            sp1B0.t[1] = (s32) sp30.y;
            sp1B0.t[2] = (s32) sp30.z;
block_158:
            func_80078CC4(var_a0, var_a1_2);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, var_s3_13 & 0xFFFF);
            func_800C71E4(D_8019B690, var_s0_2);
            func_800C6F4C(D_8019B690);
        default:                                    /* switch 2 */
            break;
        case 4:                                     /* switch 2 */
            temp_s6_4 = (effect->timer << 0xA) / 24;
            temp_s4_9 = ((s32) (0x400 - temp_s6_4) / 2) + 0x200;
            temp_s2_8 = (func_80077CF4(temp_s6_4) / 6) + 0x6AA;
            var_v1_10 = func_80077DC4(temp_s6_4);
            if (var_v1_10 < 0) {
                var_v1_10 += 0x1F;
            }
            var_s3_14 = var_v1_10 >> 5;
            if (D_800E27EC & 1) {
                temp_v0_24 = var_s3_14 * 0xF;
                var_s3_14 = temp_v0_24 >> 4;
                if (temp_v0_24 < 0) {
                    var_s3_14 = (s32) (temp_v0_24 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_8 = D_800E1204[D_800F336C];
            temp_s0_8 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_8 += 4;
            }
            func_800C6EC0(temp_s0_8, func_80077AA4(0x20, (s32) var_a2_8) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp1E0);
            sp200.x = temp_s4_9;
            sp200.y = temp_s2_8;
            sp200.z = temp_s4_9;
            sp1E0.t[0] = (s32) (s16) sp30.x;
            sp1E0.t[1] = (s32) sp30.y;
            sp1E0.t[2] = (s32) sp30.z;
            func_80078CC4(&sp1E0, &sp200);
            func_800C6EF8(D_8019B680);
            func_800C6FA0(D_8019B680, var_s3_14 & 0xFFFF);
            func_800C71E4(D_8019B680, &sp1E0);
            func_800C6F4C(D_8019B680);
            temp_s1_2 = ((effect->timer + 0x20) << 0xA) / 56;
            temp_s4_10 = (func_80077CF4(temp_s1_2) / 4) + 0xC00;
            temp_s2_9 = (func_80077DC4(temp_s1_2) / 4) + 0x400;
            var_v1_11 = func_80077DC4(temp_s1_2);
            if (var_v1_11 < 0) {
                var_v1_11 += 0x1F;
            }
            var_s3_15 = var_v1_11 >> 5;
            if (D_800E27EC & 1) {
                temp_v0_25 = var_s3_15 * 0xF;
                var_s3_15 = temp_v0_25 >> 4;
                if (temp_v0_25 < 0) {
                    var_s3_15 = (s32) (temp_v0_25 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC * -0x20;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_9 = D_800E1204[D_800F336C];
            temp_s0_9 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_9 += 4;
            }
            func_800C6EC0(temp_s0_9, func_80077AA4(0x20, (s32) var_a2_9) & 0xFFFF);
            func_800C6ED8(1);
            temp_s7_2 = sp30.y;
            sp30.y = temp_s7_2 - ((effect->timer + 0x20) * 0x18);
            func_80079754((GteShortVector *) &sp48, &sp210);
            sp230.x = temp_s4_10;
            sp230.y = temp_s2_9;
            sp230.z = temp_s4_10;
            sp210.t[0] = (s32) (s16) sp30.x;
            sp210.t[1] = (s32) sp30.y;
            sp210.t[2] = (s32) sp30.z;
            func_80078CC4(&sp210, &sp230);
            func_800C6EF8(D_8019B68C);
            func_800C6FA0(D_8019B68C, (var_s3_15 / 2) & 0xFFFF);
            func_800C71E4(D_8019B68C, &sp210);
            func_800C6F4C(D_8019B68C);
            sp30.y = temp_s7_2;
            temp_s4_11 = (func_80077CF4(temp_s1_2) / 4) + 0x1200;
            temp_s2_10 = func_80077DC4(temp_s1_2);
            var_v1_12 = func_80077DC4(temp_s1_2);
            if (var_v1_12 < 0) {
                var_v1_12 += 0x1F;
            }
            var_s3_16 = var_v1_12 >> 5;
            if (D_800E27EC & 1) {
                temp_v0_26 = var_s3_16 * 0xF;
                var_s3_16 = temp_v0_26 >> 4;
                if (temp_v0_26 < 0) {
                    var_s3_16 = (s32) (temp_v0_26 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = (D_800E27EC << 5) + 0x400;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_10 = D_800E1204[D_800F336C];
            temp_s0_10 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_10 += 4;
            }
            func_800C6EC0(temp_s0_10, func_80077AA4(0x60, (s32) var_a2_10) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp240);
            sp260.x = temp_s4_11;
            sp260.y = temp_s2_10;
            sp260.z = temp_s4_11;
            sp240.t[0] = (s32) (s16) sp30.x;
            sp240.t[1] = (s32) sp30.y;
            sp240.t[2] = (s32) sp30.z;
            func_80078CC4(&sp240, &sp260);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, var_s3_16 & 0xFFFF);
            func_800C71E4(D_8019B690, &sp240);
            func_800C6F4C(D_8019B690);
            temp_s4_12 = (func_80077CF4(temp_s1_2) / 6) + 0x1200;
            temp_s2_11 = func_80077DC4(temp_s1_2) * 2;
            var_v1_13 = func_80077DC4(temp_s1_2);
            if (var_v1_13 < 0) {
                var_v1_13 += 0x1F;
            }
            var_s3_13 = var_v1_13 >> 5;
            if (!(D_800E27EC & 1)) {
                temp_v0_27 = var_s3_13 * 0xF;
                var_s3_13 = temp_v0_27 >> 4;
                if (temp_v0_27 < 0) {
                    var_s3_13 = (s32) (temp_v0_27 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA];
            var_a2_11 = D_800E1204[D_800F336C];
            temp_s0_11 = (D_800E2850[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a2_11 += 4;
            }
            func_800C6EC0(temp_s0_11, func_80077AA4(0x60, (s32) var_a2_11) & 0xFFFF);
            func_800C6ED8(1);
            var_s0_2 = &sp270;
            func_80079754((GteShortVector *) &sp48, var_s0_2);
            var_a0 = var_s0_2;
            var_a1_2 = (GteVector *) &sp290;
            sp290.x = temp_s4_12;
            sp290.y = temp_s2_11;
            sp290.z = temp_s4_12;
            sp270.t[0] = (s32) (s16) sp30.x;
            sp270.t[1] = (s32) sp30.y;
            sp270.t[2] = (s32) sp30.z;
            goto block_158;
        }
        D_800F336A = 1;
        D_800F3368.parameter00 = 0x10;
        D_800F3376 = 0x10;
        D_800F3378 = 0x10;
        D_800F3376 = 0x80;
        D_800F3378 = 0x10;
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3370 = D_800E2850[D_800E11EA];
        goto block_161;
    }
    temp_v1_4 = effect->state;
    switch (temp_v1_4) {                            /* switch 1 */
    case 0:                                         /* switch 1 */
        temp_v0_28 = (u16) effect->timer + 1;
        effect->timer = temp_v0_28;
        if ((temp_v0_28 == 2) && (D_800B0E64 != NULL)) {
            func_8006DF50(D_800B0E64, 0x5E3, func_800D3FD8(), 0x80, 0x7F);
            if (D_800B0E64 != NULL) {
                func_8006DF50(D_800B0E64, 0x5E4, 0x80, 0x80, 0x7F);
            }
        }
        if (effect->timer >= 0x10) {
            var_v0 = 1;
block_31:
            effect->state = var_v0;
            effect->timer = 0;
        default:                                    /* switch 1 */
block_161:
        }
        return 0;
    case 1:                                         /* switch 1 */
        temp_v0_29 = (u16) effect->timer + 1;
        effect->timer = temp_v0_29;
        if (temp_v0_29 >= 0x10) {
            var_v0 = 2;
            goto block_31;
        }
        /* Duplicate return node #162. Try simplifying control flow for better match */
        return 0;
    case 2:                                         /* switch 1 */
        temp_v0_30 = (u16) effect->timer + 1;
        effect->timer = temp_v0_30;
        if (temp_v0_30 >= 0x10) {
            var_v0 = 3;
            goto block_31;
        }
        /* Duplicate return node #162. Try simplifying control flow for better match */
        return 0;
    case 3:                                         /* switch 1 */
        effect->timer = (u16) effect->timer + 1;
        if ((func_800C6B90(&effect->position, 0x960) != 0) && ((func_800C6B90(&effect->position, 0x4B0) != 0) || (func_800C6B90(&effect->position, 0x6A4) == 0)) && (M2C_FIELD(D_800E2368, u8 *, 0xD) != 0) && (((*D_800F32D0->pool)->flags & 0x3F000000) == 0x01000000)) {
            temp_v1_5 = *D_8009D254;
            M2C_FIELD(temp_v1_5, s32 *, 0x4C) = (s32) (M2C_FIELD(temp_v1_5, s32 *, 0x4C) | 0x4000);
            temp_a0 = *D_800F32D0->pool;
            temp_a0->flags = (temp_a0->flags & 0xC0FFFFFF) | 0x2D000000;
            temp_a0_2 = *D_800F32D0->pool;
            temp_a0_2->flags |= 0x80000000;
        }
        temp_a2 = effect->timer;
        if (temp_a2 < 0x11) {
            func_800D1D24(2, 0x10, (s32) temp_a2);
        }
        temp_v0_31 = func_800CE610(D_800F33E0->pool);
        if (temp_v0_31 != NULL) {
            M2C_FIELD(temp_v0_31, u16 *, 0) = (u16) effect->position.x;
            M2C_FIELD(temp_v0_31, u16 *, 2) = (u16) effect->position.y;
            M2C_FIELD(temp_v0_31, u16 *, 4) = (u16) effect->position.z;
            M2C_FIELD(temp_v0_31, u16 *, 2) = (u16) (M2C_FIELD(temp_v0_31, u16 *, 2) - (func_80071A54() % 800));
            M2C_FIELD(temp_v0_31, u16 *, 0) = (u16) (M2C_FIELD(temp_v0_31, u16 *, 0) + ((func_80071A54() % 512) - 0x100));
            temp_v1_6 = func_80071A54() % 512;
            M2C_FIELD(temp_v0_31, s16 *, 8) = 0;
            M2C_FIELD(temp_v0_31, s16 *, 0xA) = 0;
            M2C_FIELD(temp_v0_31, u16 *, 4) = (u16) (M2C_FIELD(temp_v0_31, u16 *, 4) + (temp_v1_6 - 0x100));
        }
        D_8019B668 = 0x80;
        if (effect->timer >= 0x20) {
            var_v0 = 4;
            goto block_31;
        }
        /* Duplicate return node #162. Try simplifying control flow for better match */
        return 0;
    case 4:                                         /* switch 1 */
        effect->timer = (u16) effect->timer + 1;
        temp_v0_32 = func_800CE610(D_800F33E0->pool);
        if (temp_v0_32 != NULL) {
            M2C_FIELD(temp_v0_32, u16 *, 0) = (u16) effect->position.x;
            M2C_FIELD(temp_v0_32, u16 *, 2) = (u16) effect->position.y;
            M2C_FIELD(temp_v0_32, u16 *, 4) = (u16) effect->position.z;
            M2C_FIELD(temp_v0_32, u16 *, 2) = (u16) (M2C_FIELD(temp_v0_32, u16 *, 2) - (func_80071A54() % 800));
            M2C_FIELD(temp_v0_32, u16 *, 0) = (u16) (M2C_FIELD(temp_v0_32, u16 *, 0) + ((func_80071A54() % 512) - 0x100));
            temp_v1_7 = func_80071A54() % 512;
            M2C_FIELD(temp_v0_32, s16 *, 8) = 0;
            M2C_FIELD(temp_v0_32, s16 *, 0xA) = 0;
            M2C_FIELD(temp_v0_32, u16 *, 4) = (u16) (M2C_FIELD(temp_v0_32, u16 *, 4) + (temp_v1_7 - 0x100));
        }
        var_v1_14 = func_80077DC4((effect->timer << 0xA) / 24);
        if (var_v1_14 < 0) {
            var_v1_14 += 0x1F;
        }
        D_8019B668 = var_v1_14 >> 5;
        if (effect->timer >= 0x18) {
            effect->state = 4;
            effect->timer = 0;
            return 1;
        }
        /* Duplicate return node #162. Try simplifying control flow for better match */
        return 0;
    }
}
/* Warning: struct RoomFxTransformOwner is not defined (only forward-declared) */
