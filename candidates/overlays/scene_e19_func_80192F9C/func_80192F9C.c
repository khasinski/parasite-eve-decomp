/* MASPSX_FLAGS: --expand-div */
/* Initial full-function candidate; not integrated and not yet matching. */
#include "scene_e19_recovered.h"
#include "pe1/gte.h"
#include "m2c_macros.h"
#define NULL ((void *)0)
extern u16 D_800942EC;
extern void **D_8009D254;
#include "pe1/room_sound_slot.h"
extern RoomSoundSlot D_800B0E64;
extern u16 D_800E11EA[1];
extern u16 D_800E11FA[1];
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

/* Keep GPU argument narrowing explicit at the inline boundary (candidate debt). */
static __inline__ u16 gpuWord(u16 value) { return value; }

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
    s16 var_v0;
    int spread;
    int ringRadius;
    const u32 *matrix;
    register u32 w0 asm("$12");
    register u32 w1 asm("$13");
    register u32 w2 asm("$14");
    s32 texturePage;
    s32 modelPhase;
    s32 temp_s2;
    s32 verticalScale;
    s32 radialScale;
    s32 phase;
    s32 temp_v0_10;
    s32 temp_v0_11;
    s32 temp_v0_12;
    s32 temp_v0_13;
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
    s32 intensity;
    s32 var_s4;
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
    u16 paletteRow;
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
    switch (mode) {
    case 0: {
            temp_v0 = func_8006E498(D_800B0E64.channel, 0xC54C0704U);
            D_8019B680 = temp_v0;
            func_800C6D5C(temp_v0, 0U, 0U);
            temp_v0_2 = func_8006E498(D_800B0E64.channel, 0xC58C0704U);
            D_8019B684 = temp_v0_2;
            func_800C6D5C(temp_v0_2, 0U, 0U);
            temp_v0_3 = func_8006E498(D_800B0E64.channel, 0xC5CC0704U);
            D_8019B688 = temp_v0_3;
            func_800C6D5C(temp_v0_3, 0U, 0U);
            temp_v0_4 = func_8006E498(D_800B0E64.channel, 0xC60C0704U);
            D_8019B68C = temp_v0_4;
            func_800C6D5C(temp_v0_4, 0U, 0U);
            temp_v0_5 = func_8006E498(D_800B0E64.channel, 0xC64C0704U);
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
    case 1: {
    temp_v1_4 = effect->state;
    switch (temp_v1_4) {                            /* switch 1 */
    case 0:                                         /* switch 1 */
        temp_v0_28 = (u16) effect->timer + 1;
        effect->timer = temp_v0_28;
        if ((temp_v0_28 == 2) && (D_800B0E64.channel != NULL)) {
            func_8006DF50(D_800B0E64.channel, 0x5E3, func_800D3FD8(), 0x80, 0x7F);
            if (D_800B0E64.channel != NULL) {
                func_8006DF50(D_800B0E64.channel, 0x5E4, 0x80, 0x80, 0x7F);
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
        spread = 512;
        asm("" : "=r"(spread) : "0"(spread));
        temp_v0_31 = func_800CE610(D_800F33E0->pool);
        if (temp_v0_31 != NULL) {
            M2C_FIELD(temp_v0_31, u16 *, 0) = (u16) effect->position.x;
            M2C_FIELD(temp_v0_31, u16 *, 2) = (u16) effect->position.y;
            M2C_FIELD(temp_v0_31, u16 *, 4) = (u16) effect->position.z;
            M2C_FIELD(temp_v0_31, u16 *, 2) = (u16) (M2C_FIELD(temp_v0_31, u16 *, 2) - (func_80071A54() % 800));
            M2C_FIELD(temp_v0_31, u16 *, 0) = (u16) (M2C_FIELD(temp_v0_31, u16 *, 0) + ((func_80071A54() % spread) - 0x100));
            temp_v1_6 = func_80071A54() % spread;
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
        spread = 512;
        asm("" : "=r"(spread) : "0"(spread));
        temp_v0_32 = func_800CE610(D_800F33E0->pool);
        if (temp_v0_32 != NULL) {
            M2C_FIELD(temp_v0_32, u16 *, 0) = (u16) effect->position.x;
            M2C_FIELD(temp_v0_32, u16 *, 2) = (u16) effect->position.y;
            M2C_FIELD(temp_v0_32, u16 *, 4) = (u16) effect->position.z;
            M2C_FIELD(temp_v0_32, u16 *, 2) = (u16) (M2C_FIELD(temp_v0_32, u16 *, 2) - (func_80071A54() % 800));
            M2C_FIELD(temp_v0_32, u16 *, 0) = (u16) (M2C_FIELD(temp_v0_32, u16 *, 0) + ((func_80071A54() % spread) - 0x100));
            temp_v1_7 = func_80071A54() % spread;
            M2C_FIELD(temp_v0_32, s16 *, 8) = 0;
            M2C_FIELD(temp_v0_32, s16 *, 0xA) = 0;
            M2C_FIELD(temp_v0_32, u16 *, 4) = (u16) (M2C_FIELD(temp_v0_32, u16 *, 4) + (temp_v1_7 - 0x100));
        }
        var_v1_14 = func_80077DC4((effect->timer << 0xA) / 24);
        D_8019B668 = var_v1_14 / 32;
        if (effect->timer >= 0x18) {
            effect->state = 4;
            effect->timer = 0;
            return 1;
        }
        /* Duplicate return node #162. Try simplifying control flow for better match */
        return 0;
    }
    }
    case 2: {
        sp30.x = (u16) effect->position.x;
        sp30.y = (s16) (u16) effect->position.y;
        sp30.z = (s16) (u16) effect->position.z;
        {
            /* Keep position stores before the first GTE matrix load. */
            asm volatile("" : : : "memory");
            matrix = (const u32 *)D_800BCFA4.value;
            w0 = matrix[0];
            w1 = matrix[1];
            gte_ctc2_0(w0);
            gte_ctc2_1(w1);
            w0 = matrix[2];
            w1 = matrix[3];
            w2 = matrix[4];
            gte_ctc2_2(w0);
            gte_ctc2_3(w1);
            gte_ctc2_4(w2);
            w0 = matrix[5];
            w1 = matrix[6];
            gte_ctc2_5(w0);
            w2 = matrix[7];
            gte_ctc2_6(w1);
            gte_ctc2_7(w2);
        }
        D_800F3372 = 0;
        D_800F3374 = 4;
        temp_v1 = effect->state;
        switch (temp_v1) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
            temp_v0_6 = func_80077CF4(effect->timer << 6);
            intensity = temp_v0_6 >> 5;
            if (temp_v0_6 < 0) {
                intensity = (s32) (temp_v0_6 + 0x1F) >> 5;
            }
            D_800F3368.parameter00 = 0x40;
            D_800F336A = 4;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3370 = D_800E2850[D_800E11FA[0]];
            if (D_800E27EC & 1) {
                intensity = (intensity * 2) / 3;
            }
            sp68.pad = 1;
            sp68.x = 0x400;
            sp68.y = 0;
            sp68.z = 0;
            sp60.x = (u16) sp30.x;
            sp60.z = (s16) (u16) sp30.z;
            sp60.y = (s16) D_800942EC;
            func_800CEE20(&sp60, (GteRotation *) &sp68, 0x2000, 0x2000, 0, gpuWord(func_80077AA4(0, D_800E120A + 2)), 1, intensity, NULL);
            break;
        case 1:                                     /* switch 2 */
            intensity = 0x80;
            D_800F336A = 4;
            D_800F3368.parameter00 = 0x40;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3370 = D_800E2850[D_800E11FA[0]];
            phase = effect->timer << 6;
            if (D_800E27EC & 1) {
                intensity = 0x55;
            }
            sp78.pad = 1;
            sp78.x = 0x400;
            sp78.y = 0;
            sp78.z = 0;
            sp70.x = (u16) sp30.x;
            sp70.z = (s16) (u16) sp30.z;
            sp70.y = (s16) D_800942EC;
            func_800CEE20(&sp70, (GteRotation *) &sp78, 0x2000, 0x2000, 0, gpuWord(func_80077AA4(0, D_800E120A + 2)), 1, intensity, NULL);
            var_v1 = func_80077CF4(phase);
            intensity = var_v1 / 32;
            if (D_800E27EC & 1) {
                intensity = (intensity * 2) / 3;
            }
            var_s4 = func_80077CF4(phase);
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
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) paletteRow)));
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
            {
                matrix = (const u32 *)D_800BCFA4.value;
                w0 = matrix[0];
                w1 = matrix[1];
                gte_ctc2_0(w0);
                gte_ctc2_1(w1);
                w0 = matrix[2];
                w1 = matrix[3];
                w2 = matrix[4];
                gte_ctc2_2(w0);
                gte_ctc2_3(w1);
                gte_ctc2_4(w2);
                w0 = matrix[5];
                w1 = matrix[6];
                gte_ctc2_5(w0);
                w2 = matrix[7];
                gte_ctc2_6(w1);
                gte_ctc2_7(w2);
            }
            func_800D004C(&sp30, 0x12C, 0x12C, 0xC, NULL, 0x1000, 0x1000, &sp50, NULL, intensity, 1);
            temp_v0_9 = func_80077DC4(phase);
            intensity = temp_v0_9 >> 5;
            if (temp_v0_9 < 0) {
                intensity = (s32) (temp_v0_9 + 0x1F) >> 5;
            }
            temp_v0_10 = func_80077CF4(phase);
            func_800D0728(&sp30, 0x7D0, 0xB54, 0x18, &sp40, temp_v0_10, temp_v0_10, NULL, &sp50, intensity, 1);
            break;
        case 2:                                     /* switch 2 */
            intensity = 0x80;
            D_800F3368.parameter00 = 0x40;
            D_800F336A = 4;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F3370 = D_800E2850[D_800E11FA[0]];
            D_800F336C = 3;
            D_800F336E = 1;
            phase = effect->timer << 6;
            if (D_800E27EC & 1) {
                intensity = 0x55;
            }
            spB8.pad = 1;
            spB8.x = 0x400;
            var_s4_2 = 0x1000;
            spB8.y = 0;
            spB8.z = 0;
            spB0.x = (u16) sp30.x;
            spB0.z = (s16) (u16) sp30.z;
            spB0.y = (s16) D_800942EC;
            func_800CEE20(&spB0, (GteRotation *) &spB8, 0x2000, 0x2000, 0, gpuWord(func_80077AA4(0, D_800E120A + 2)), 1, intensity, NULL);
            if (D_800E27EC & 1) {
                var_s4_2 = 0xF00;
            }
            sp48.x = -0x400;
            sp48.y = 0;
            sp48.z = D_800E27EC << 5;
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            var_v1_2 = D_800E1204[D_800F3368.palette];
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0)));
            if ((D_800F3368.palette == 4) && (D_800F3428 != 0)) {
                var_v1_2 += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) var_v1_2)));
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
            {
                matrix = (const u32 *)D_800BCFA4.value;
                w0 = matrix[0];
                w1 = matrix[1];
                gte_ctc2_0(w0);
                gte_ctc2_1(w1);
                w0 = matrix[2];
                w1 = matrix[3];
                w2 = matrix[4];
                gte_ctc2_2(w0);
                gte_ctc2_3(w1);
                gte_ctc2_4(w2);
                w0 = matrix[5];
                w1 = matrix[6];
                gte_ctc2_5(w0);
                w2 = matrix[7];
                gte_ctc2_6(w1);
                gte_ctc2_7(w2);
            }
            func_800D004C(&sp30, 0x12C, 0x12C, 0xC, NULL, 0x1000, 0x1000, &sp50, NULL, intensity, 1);
            intensity = 0x80;
            if (D_800E27EC & 1) {
                intensity = 0x64;
            }
            ringRadius = 2000;
            /* Keep retail register multiplication; see candidate debt. */
            asm("" : "=r"(ringRadius) : "0"(ringRadius));
            temp_v0_12 = func_80077DC4(phase);
            radialScale = temp_v0_12 >> 3;
            if (temp_v0_12 < 0) {
                radialScale = (s32) (temp_v0_12 + 7) >> 3;
            }
            temp_v0_13 = func_80077CF4(phase);
            verticalScale = (temp_v0_13 * 2) / 3;
            sp48.x = 0x400;
            sp48.y = 0;
            sp48.z = 0;
            for (var_s0 = 0; var_s0 < 0x10; ++var_s0) {
                temp_s2 = (var_s0 << 8) + (D_800E27EC * 4);
                sp38.z = (s16) (u16) sp30.z;
                sp38.x = (u16) sp30.x;
                sp38.y = (s16) (u16) sp30.y;
                var_v1_4 = func_80077DC4(temp_s2) * ringRadius;
                if (var_v1_4 < 0) {
                    var_v1_4 += 0xFFF;
                }
                sp38.x = (u16) sp38.x + (var_v1_4 >> 0xC);
                var_v1_5 = func_80077CF4(temp_s2) * ringRadius;
                if (var_v1_5 < 0) {
                    var_v1_5 += 0xFFF;
                }
                sp48.z = temp_s2 + 0x400;
                sp38.z = (u16) sp38.z + (var_v1_5 >> 0xC);
                func_800D0E88((GteShortVector *) &sp38, (GteRotation *) &sp48, verticalScale, radialScale, &sp50, NULL, NULL, (s32) (s16) intensity, 1);
            }
            temp_v0_15 = func_80077DC4(phase);
            intensity = temp_v0_15 >> 5;
            if (temp_v0_15 < 0) {
                intensity = (s32) (temp_v0_15 + 0x1F) >> 5;
            }
            radialScale = func_80077DC4(phase);
            temp_s2_2 = &effect->endpoint;
            temp_v1_2 = (u16) effect->origin.x;
            sp48.x = temp_v1_2;
            temp_v0_16 = (u16) effect->origin.y;
            sp48.y = temp_v0_16;
            sp48.x = temp_v1_2 - 0x200;
            sp48.y = temp_v0_16 + 0x400;
            sp48.z = (u16) effect->origin.z;
            func_800D0728(temp_s2_2, 0x7D0, 0xA8C, 0x20, (GteRotation *) &sp48, radialScale, radialScale, &sp58, NULL, intensity, 1);
            sp48.x += 0x400;
            func_800D0728(temp_s2_2, 0x7D0, 0xA8C, 0x20, (GteRotation *) &sp48, radialScale, radialScale, &sp58, NULL, intensity, 1);
            break;
        case 3:                                     /* switch 2 */
            temp_v1_3 = effect->timer;
            phase = temp_v1_3 << 5;
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
            temp_v0_17 = func_80077DC4(phase);
            radialScale = ((s32) temp_v0_17 / 2) + 0x400;
            verticalScale = (func_80077CF4(phase) / 6) + 0x555;
            intensity = 0x80;
            if (D_800E27EC & 1) {
                intensity = 0x78;
            }
            sp48.x = 0;
            sp48.y = D_800E27EC << 5;
            sp48.z = 0;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) paletteRow)));
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &spF0);
            sp110.x = radialScale;
            sp110.y = verticalScale;
            sp110.z = radialScale;
            spF0.t[0] = (s32) (s16) sp30.x;
            spF0.t[1] = (s32) sp30.y;
            spF0.t[2] = (s32) sp30.z;
            func_80078CC4(&spF0, &sp110);
            func_800C6EF8(D_8019B680);
            func_800C6FA0(D_8019B680, gpuWord(intensity));
            func_800C71E4(D_8019B680, &spF0);
            func_800C6F4C(D_8019B680);
            radialScale = (func_80077CF4(phase) / 3) + 0x400;
            temp_v0_18 = func_80077DC4(phase);
            verticalScale = (s32) temp_v0_18 / 2;
            var_v1_6 = func_80077DC4(phase);
            intensity = var_v1_6 / 32;
            if (D_800E27EC & 1) {
                temp_v0_19 = intensity * 0xF;
                intensity = temp_v0_19 >> 4;
                if (temp_v0_19 < 0) {
                    intensity = (s32) (temp_v0_19 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC * -0x30;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) paletteRow)));
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp120);
            sp140.x = radialScale;
            sp140.y = verticalScale;
            sp140.z = radialScale;
            sp120.t[0] = (s32) (s16) sp30.x;
            sp120.t[1] = (s32) sp30.y;
            sp120.t[2] = (s32) sp30.z;
            func_80078CC4(&sp120, &sp140);
            func_800C6EF8(D_8019B688);
            func_800C6FA0(D_8019B688, gpuWord((intensity / 2)));
            func_800C71E4(D_8019B688, &sp120);
            func_800C6F4C(D_8019B688);
            modelPhase = (effect->timer << 0xA) / 56;
            radialScale = (func_80077CF4(modelPhase) / 4) + 0xC00;
            verticalScale = (func_80077DC4(modelPhase) / 4) + 0x400;
            var_v1_7 = func_80077DC4(modelPhase);
            intensity = var_v1_7 / 32;
            if (D_800E27EC & 1) {
                temp_v0_20 = intensity * 0xF;
                intensity = temp_v0_20 >> 4;
                if (temp_v0_20 < 0) {
                    intensity = (s32) (temp_v0_20 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC * -0x20;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) paletteRow)));
            func_800C6ED8(1);
            temp_s7 = sp30.y;
            sp30.y = temp_s7 - (effect->timer * 0x18);
            func_80079754((GteShortVector *) &sp48, &sp150);
            sp170.x = radialScale;
            sp170.y = verticalScale;
            sp170.z = radialScale;
            sp150.t[0] = (s32) (s16) sp30.x;
            sp150.t[1] = (s32) sp30.y;
            sp150.t[2] = (s32) sp30.z;
            func_80078CC4(&sp150, &sp170);
            func_800C6EF8(D_8019B68C);
            func_800C6FA0(D_8019B68C, gpuWord((intensity / 2)));
            func_800C71E4(D_8019B68C, &sp150);
            func_800C6F4C(D_8019B68C);
            sp30.y = temp_s7;
            {
                matrix = (const u32 *)D_800BCFA4.value;
                w0 = matrix[0];
                w1 = matrix[1];
                gte_ctc2_0(w0);
                gte_ctc2_1(w1);
                w0 = matrix[2];
                w1 = matrix[3];
                w2 = matrix[4];
                gte_ctc2_2(w0);
                gte_ctc2_3(w1);
                gte_ctc2_4(w2);
                w0 = matrix[5];
                w1 = matrix[6];
                gte_ctc2_5(w0);
                w2 = matrix[7];
                gte_ctc2_6(w1);
                gte_ctc2_7(w2);
            }
            func_800D004C(&sp30, 0x9C4, 0x9C4, 0xC, NULL, 0x1000, 0x1000, &sp50, NULL, intensity, 1);
            radialScale = (func_80077CF4(phase) / 4) + 0xC00;
            temp_v0_21 = func_80077DC4(phase);
            intensity = temp_v0_21 >> 5;
            if (temp_v0_21 < 0) {
                intensity = (s32) (temp_v0_21 + 0x1F) >> 5;
            }
            func_800D0728(&sp30, 0x76C, 0xA28, 0x18, &sp40, radialScale, radialScale, &sp58, NULL, intensity, 1);
            sp30.y = (u16) sp30.y - 0x400;
            radialScale = ((s32) (func_80077CF4(phase) * 3) / 2) + 0x1000;
            func_800D0728(&sp30, 0x3E8, 0x5DC, 0x18, &sp40, radialScale, radialScale, &sp50, NULL, intensity, 1);
            sp30.y = (u16) sp30.y + 0x400;
            radialScale = (func_80077CF4(modelPhase) / 4) + 0x1200;
            verticalScale = func_80077DC4(modelPhase);
            var_v1_8 = func_80077DC4(modelPhase);
            intensity = var_v1_8 / 32;
            if (D_800E27EC & 1) {
                temp_v0_22 = intensity * 0xF;
                intensity = temp_v0_22 >> 4;
                if (temp_v0_22 < 0) {
                    intensity = (s32) (temp_v0_22 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = (D_800E27EC << 5) + 0x400;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x60, (s32) paletteRow)));
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp180);
            sp1A0.x = radialScale;
            sp1A0.y = verticalScale;
            sp1A0.z = radialScale;
            sp180.t[0] = (s32) (s16) sp30.x;
            sp180.t[1] = (s32) sp30.y;
            sp180.t[2] = (s32) sp30.z;
            func_80078CC4(&sp180, &sp1A0);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, gpuWord(intensity));
            func_800C71E4(D_8019B690, &sp180);
            func_800C6F4C(D_8019B690);
            radialScale = (func_80077CF4(modelPhase) / 6) + 0x1200;
            verticalScale = func_80077DC4(modelPhase) * 2;
            var_v1_9 = func_80077DC4(modelPhase);
            intensity = var_v1_9 / 32;
            if (!(D_800E27EC & 1)) {
                temp_v0_23 = intensity * 0xF;
                intensity = temp_v0_23 >> 4;
                if (temp_v0_23 < 0) {
                    intensity = (s32) (temp_v0_23 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x60, (s32) paletteRow)));
            func_800C6ED8(1);
            var_s0_2 = &sp1B0;
            func_80079754((GteShortVector *) &sp48, var_s0_2);
            var_a0 = var_s0_2;
            var_a1_2 = &sp1D0;
            sp1D0.x = radialScale;
            sp1D0.y = verticalScale;
            sp1D0.z = radialScale;
            sp1B0.t[0] = (s32) (s16) sp30.x;
            sp1B0.t[1] = (s32) sp30.y;
            sp1B0.t[2] = (s32) sp30.z;
block_158:
            func_80078CC4(var_a0, var_a1_2);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, gpuWord(intensity));
            func_800C71E4(D_8019B690, var_s0_2);
            func_800C6F4C(D_8019B690);
        default:                                    /* switch 2 */
            break;
        case 4:                                     /* switch 2 */
            phase = (effect->timer << 0xA) / 24;
            radialScale = ((s32) (0x400 - phase) / 2) + 0x200;
            verticalScale = (func_80077CF4(phase) / 6) + 0x6AA;
            var_v1_10 = func_80077DC4(phase);
            intensity = var_v1_10 / 32;
            if (D_800E27EC & 1) {
                temp_v0_24 = intensity * 0xF;
                intensity = temp_v0_24 >> 4;
                if (temp_v0_24 < 0) {
                    intensity = (s32) (temp_v0_24 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) paletteRow)));
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp1E0);
            sp200.x = radialScale;
            sp200.y = verticalScale;
            sp200.z = radialScale;
            sp1E0.t[0] = (s32) (s16) sp30.x;
            sp1E0.t[1] = (s32) sp30.y;
            sp1E0.t[2] = (s32) sp30.z;
            func_80078CC4(&sp1E0, &sp200);
            func_800C6EF8(D_8019B680);
            func_800C6FA0(D_8019B680, gpuWord(intensity));
            func_800C71E4(D_8019B680, &sp1E0);
            func_800C6F4C(D_8019B680);
            modelPhase = ((effect->timer + 0x20) << 0xA) / 56;
            radialScale = (func_80077CF4(modelPhase) / 4) + 0xC00;
            verticalScale = (func_80077DC4(modelPhase) / 4) + 0x400;
            var_v1_11 = func_80077DC4(modelPhase);
            intensity = var_v1_11 / 32;
            if (D_800E27EC & 1) {
                temp_v0_25 = intensity * 0xF;
                intensity = temp_v0_25 >> 4;
                if (temp_v0_25 < 0) {
                    intensity = (s32) (temp_v0_25 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC * -0x20;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x20, (s32) paletteRow)));
            func_800C6ED8(1);
            temp_s7_2 = sp30.y;
            sp30.y = temp_s7_2 - ((effect->timer + 0x20) * 0x18);
            func_80079754((GteShortVector *) &sp48, &sp210);
            sp230.x = radialScale;
            sp230.y = verticalScale;
            sp230.z = radialScale;
            sp210.t[0] = (s32) (s16) sp30.x;
            sp210.t[1] = (s32) sp30.y;
            sp210.t[2] = (s32) sp30.z;
            func_80078CC4(&sp210, &sp230);
            func_800C6EF8(D_8019B68C);
            func_800C6FA0(D_8019B68C, gpuWord((intensity / 2)));
            func_800C71E4(D_8019B68C, &sp210);
            func_800C6F4C(D_8019B68C);
            sp30.y = temp_s7_2;
            radialScale = (func_80077CF4(modelPhase) / 4) + 0x1200;
            verticalScale = func_80077DC4(modelPhase);
            var_v1_12 = func_80077DC4(modelPhase);
            intensity = var_v1_12 / 32;
            if (D_800E27EC & 1) {
                temp_v0_26 = intensity * 0xF;
                intensity = temp_v0_26 >> 4;
                if (temp_v0_26 < 0) {
                    intensity = (s32) (temp_v0_26 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = (D_800E27EC << 5) + 0x400;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x60, (s32) paletteRow)));
            func_800C6ED8(1);
            func_80079754((GteShortVector *) &sp48, &sp240);
            sp260.x = radialScale;
            sp260.y = verticalScale;
            sp260.z = radialScale;
            sp240.t[0] = (s32) (s16) sp30.x;
            sp240.t[1] = (s32) sp30.y;
            sp240.t[2] = (s32) sp30.z;
            func_80078CC4(&sp240, &sp260);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, gpuWord(intensity));
            func_800C71E4(D_8019B690, &sp240);
            func_800C6F4C(D_8019B690);
            radialScale = (func_80077CF4(modelPhase) / 6) + 0x1200;
            verticalScale = func_80077DC4(modelPhase) * 2;
            var_v1_13 = func_80077DC4(modelPhase);
            intensity = var_v1_13 / 32;
            if (!(D_800E27EC & 1)) {
                temp_v0_27 = intensity * 0xF;
                intensity = temp_v0_27 >> 4;
                if (temp_v0_27 < 0) {
                    intensity = (s32) (temp_v0_27 + 0xF) >> 4;
                }
            }
            sp48.x = 0;
            sp48.z = 0;
            sp48.y = D_800E27EC << 5;
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[D_800E11EA[0]];
            texturePage = func_80077A64(0, 1, 0, 0);
            texturePage = gpuWord((D_800E2850[D_800E11EA[0]] | texturePage));
            paletteRow = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                paletteRow += 4;
            }
            func_800C6EC0(texturePage, gpuWord(func_80077AA4(0x60, (s32) paletteRow)));
            func_800C6ED8(1);
            var_s0_2 = &sp270;
            func_80079754((GteShortVector *) &sp48, var_s0_2);
            var_a0 = var_s0_2;
            var_a1_2 = (GteVector *) &sp290;
            sp290.x = radialScale;
            sp290.y = verticalScale;
            sp290.z = radialScale;
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
        D_800F3370 = D_800E2850[D_800E11EA[0]];
        goto block_161;
    }
    }
    return 0;
}
