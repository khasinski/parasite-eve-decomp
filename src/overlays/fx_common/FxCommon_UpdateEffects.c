#include "fx_common_motion.h"

/* Advance effect selection, resource transitions and the two motion blends. */
void func_801942FC(void)
{
    FxCommonMotionVec mode1Sample;
    FxCommonMotionVec mode1NextSample;
    FxCommonMotionVec mode2Sample;
    FxCommonMotionVec mode2NextSample;
    FxCommonShortVec shortA;
    FxCommonShortVec shortB;
    FxCommonShortVec sampleExtra;
    FxCommonMotionVec motionSample;
    FxCommonShortVec motionExtra;
    s32 copyX;
    s32 copyZ;
    s32 currentX;
    s32 currentY;
    /* Retains the retail register order for the short-vector copy. */
    register s32 currentZ asm("$10");
    s16 resourceResult;
    s32 rowIndex;
    s32 rowWords;
    /* Raw halfword is loaded before the blend duration, then sign-extended. */
    register u16 phaseBits asm("$2");
    s16 nextMode;
    s16 remaining0;
    s16 remaining1;
    s16 motionBase0;
    s16 motionBase1;
    s16 countdown;
    s16 mode;
    s16 transitionMode;
    s32 setupByteOffset;
    s32 accumX0;
    s32 accumX1;
    s32 accumZ0;
    s32 accumZ1;
    s32 nextFrame;
    s32 elapsed;
    s32 resourceTicks;
    s32 accumY1;
    s32 nextSetupOffset0;
    s32 nextSetupOffset1;
    s32 accumY0;
    s32 motionOffset0;
    s32 motionOffset1;
    u32 motionIndex0;
    u32 motionIndex1;
    s16 nextIndex0;
    s16 nextIndex1;

    resourceResult = 0;
    if (D_8019C026 != 0) {
        countdown = D_8019C026 - 1;
        D_8019C026 = countdown;
        if (countdown == 1) {
            D_8019C024 = 1;
        }
    }
    if (D_8019C045 == 1) {
        func_80195994(D_8019CC52, 0, 0, D_8019CC1C << 8);
        rowIndex = D_8019CC52;
        rowWords = rowIndex * (sizeof(FxCommonEffectSetupRecord) / sizeof(s32));
        setupByteOffset = rowWords << 2;
        if (D_8019CC1C >= ((FxCommonEffectSetupRecord *)((u8 *)g_FxCommonEffectSetup + setupByteOffset))->value30) {
            D_8019C032 = 0;
        }
        if (D_8019CC1C < (((FxCommonEffectSetupRecord *)((u8 *)g_FxCommonEffectSetup + setupByteOffset))->value0c - 1)) {
            D_8019CC1C += 1;
        } else {
            D_8019C045 = 0;
        }
    }
    if ((D_8019C026 != 0) && (D_8019C045 == 0) && (D_8019C034 != 0xA) && (D_8019C046 == 0) && (D_8019BFF8 == -1)) {
        nextFrame = D_8019CC1C + 1;
        D_8019CC1C = nextFrame;
        func_80195994(D_8019CC52, 0, 0, nextFrame << 8);
    }
    if ((D_8019C044 == 1) && (elapsed = D_8019C048 + 1, D_8019C048 = elapsed, ((elapsed < 0xC9) == 0)) && (D_8019C040 == 0)) {
        if (D_8019C046 == 0) {
            D_8019C04C = 0;
        }
        D_8019C046 = 1;
    } else {
        D_8019C046 = 0;
    }
    mode = D_8019C034;
    if (mode != 0xA) {
        if ((D_8019C026 == 0) && (D_8009D26C != 0)) {
            if ((D_8009D1F4 & 2) && (D_8019BFF8 == -1)) {
                nextMode = (s16)(mode + 1) % 3;
                D_8019C034 = nextMode;
                if ((nextMode << 0x10) == 0) {
                    D_8019C02C = 1;
                    D_8019C045 = 0;
                    func_80195994(D_8019CC52, 4, 5, 0);
                    if ((D_8019C026 == 0) && (D_800B0E08[0] != 0)) {
                        func_8006DF50((void *) D_800B0E08[0], 0x44D, 0, 0x80, 0x7F);
                    }
                }
                transitionMode = D_8019C034;
                if (transitionMode == 1) {
                    if (D_8009D26C & 0x20) {
                        if ((D_8019C026 == 0) && (D_800B0E08[0] != 0)) {
                            func_8006DF50((void *) D_800B0E08[0], 0x44C, 0, 0x80, 0x7F);
                        }
                        D_8019C02C = -1;
                        D_8019C032 = 1;
                        D_8019C048 = 0;
                        D_8019C044 = 0;
                        D_8019C045 = 0;
                        D_8019C13C = 0;
                        func_8018F55C(D_8019C038 + 0x20, 0x4A, func_8006EC6C((&D_801D0260), 2), &mode1Sample, &sampleExtra);
                        func_8018F55C(D_8019C038 + 0x20, 0x4B, func_8006EC6C((&D_801D0260), 2), &mode1NextSample, &sampleExtra);
                        copyX = mode1Sample.x;
                        copyZ = mode1Sample.z;
                        currentX = *(s32 *)&g_FxCommonMotionPosition0.x;
                        currentY = g_FxCommonMotionPosition0.y;
                        currentZ = g_FxCommonMotionPosition0.z;
                        shortA.x = (s16)copyX;
                        shortA.y = mode1Sample.y - 0x64;
                        shortA.z = (s16)copyZ;
                        shortB.x = (s16)currentX;
                        shortB.y = (s16)currentY;
                        shortB.z = (s16)currentZ;
                        func_80195BC8((FxCommonShortVec3 *)&shortA, (FxCommonShortVec3 *)&shortB, 4, 5);
                        D_8019C13E = 1;
                    } else {
                        D_8019C034 = 2;
                        D_8019C13E = 0;
                    }
                }
                if (D_8019C034 == 2) {
                    if ((D_8019C026 == 0) && (D_800B0E08[0] != 0)) {
                        func_8006DF50((void *) D_800B0E08[0], 0x44C, 0, 0x80, 0x7F);
                    }
                    if (D_8019C13E == 0) {
                        D_8019C02C = -1;
                    }
                    if (D_8019C13E == 1) {
                        D_8019C02C = 0;
                    }
                    D_8019C032 = 1;
                    D_8019C048 = 0;
                    D_8019C044 = 0;
                    D_8019C045 = 0;
                    D_8019C13C = 0;
                    func_8018F55C(D_8019C03C + 0x100, 0x4C, func_8006EC6C((&D_801D0260), 2), &mode2Sample, &sampleExtra);
                    func_8018F55C(D_8019C03C + 0x100, 0x4D, func_8006EC6C((&D_801D0260), 2), &mode2NextSample, &sampleExtra);
                    shortA.x = (s16) mode2NextSample.x;
                    shortA.y = (s16) mode2NextSample.y;
                    shortA.z = (s16) mode2NextSample.z;
                    shortB.x = (s16) mode2Sample.x;
                    shortB.y = (s16) mode2Sample.y;
                    shortB.z = (s16) mode2Sample.z;
                    func_80195BC8((FxCommonShortVec3 *)&shortA, (FxCommonShortVec3 *)&shortB, 4, 5);
                }
            }
            if ((D_8009D1F4 & 0x50) && (D_8019C034 != 0)) {
                D_8019C034 = 0;
                D_8019C02C = 1;
                D_8019C045 = 0;
                func_80195994(D_8019CC52, 4, 5, 0);
                if ((D_8019C026 == 0) && (D_800B0E08[0] != 0)) {
                    func_8006DF50((void *) D_800B0E08[0], 0x44D, 0, 0x80, 0x7F);
                }
            }
            if (((u8) D_8019C040 < 2U) && (D_8019C034 == 0) && (D_8019C13C == 0)) {
                if ((D_8009D1F4 & 0x20000000) && (D_8019C026 == 0)) {
                    if (D_8019C040 == 1) {
                        D_8019C02A = 1;
                        D_8019C026 = 0x10;
                        D_8019CA68 = D_8019CC52;
                    }
                    if (D_8019C044 == 0) {
                        if (D_800B0E08[0] != 0) {
                            func_8006DF50((void *) D_800B0E08[0], 0x44C, 0, 0x80, 0x7F);
                        }
                        D_8019C02C = -1;
                        D_8019C045 = 1;
                        D_8019C044 = 1;
                        D_8019CC1C = 0;
                        D_8019C048 = 1;
                    } else {
                        if (D_800B0E08[0] != 0) {
                            func_8006DF50((void *) D_800B0E08[0], 0x44C, 0, 0x80, 0x7F);
                        }
                        D_8019C02A = 1;
                        D_8019C026 = 0x10;
                        D_8019CA68 = D_8019CC52;
                    }
                    if ((D_8019C000 == 1) && (D_8019CC52 == D_8019C004)) {
                        D_8019C02C = -1;
                        D_8019C02A = 1;
                        D_8019C045 = 0;
                        D_8019C044 = 0;
                        D_8019CC1C = 0;
                        D_8019C048 = 0;
                        D_8019C026 = 0x10;
                        D_8019CA68 = D_8019CC52;
                    }
                }
                if ((D_8009D1F4 & 0x40000000) && (D_8019BFF8 == -1) && (D_8019C026 == 0)) {
                    D_8019C032 = 1;
                    if (D_8019C044 == 1) {
                        if (D_800B0E08[0] != 0) {
                            func_8006DF50((void *) D_800B0E08[0], 0x44D, 0, 0x80, 0x7F);
                        }
                        func_80195994(D_8019CC52, 4, 5, 0);
                        D_8019C02C = 1;
                        D_8019C044 = 0;
                        D_8019C045 = 0;
                    }
                }
                if ((D_8009D1F4 & 0x40) && (D_8019BFF8 == -1)) {
                    if (D_8019C044 == 1) {
                        D_8019C02C = 1;
                    }
                    D_8019C045 = 0;
                    D_8019C044 = 0;
                    D_8019CC1C = 0;
                    D_8019C032 = 1;
                    if (g_FxCommonEffectSetup[D_8019CC52].value22 != 0xFF) {
                        do {
                            nextIndex0 = g_FxCommonEffectSetup[D_8019CC52].value22;
                            D_8019CC52 = (s16) nextIndex0;
                            nextSetupOffset0 = nextIndex0 * 0x34;
                        } while (((FxCommonEffectSetupRecord *)((u8 *)g_FxCommonEffectSetup + nextSetupOffset0))->enabled == 0);
                        D_8019C018 = 1;
                        D_8019CC1C = ((FxCommonEffectSetupRecord *)((u8 *)g_FxCommonEffectSetup + nextSetupOffset0))->value0c;
                        func_80195994(nextIndex0, 4, 5, 0);
                        motionIndex0 = 0;
                        motionOffset0 = 0;
                        phaseBits = D_8019CC52;
                        D_8019C058 = 0x20;
                        motionBase0 = (s16)phaseBits;
                        do {
                            func_8018F55C(motionIndex0 << 8, motionBase0 + 0x40, func_8006EC6C((&D_801D0260), 2), &motionSample, &motionExtra);
                            motionIndex0 += 1;
                            *(s32 *)((u8 *)D_801EA268 + motionOffset0) = ((motionSample.x << 0x10) - *(s32 *)((u8 *)D_8019CAA8 + motionOffset0)) >> 5;
                            *(s32 *)((u8 *)D_801EA26C + motionOffset0) = ((motionSample.y << 0x10) - *(s32 *)((u8 *)D_8019CAAC + motionOffset0)) >> 5;
                            *(s32 *)((u8 *)D_801EA270 + motionOffset0) = ((motionSample.z << 0x10) - *(s32 *)((u8 *)D_8019CAB0 + motionOffset0)) >> 5;
                            motionOffset0 += 0x10;
                        } while (motionIndex0 < 0xAU);
                    }
                }
                if ((D_8009D1F4 & 0x10) && (D_8019BFF8 == -1)) {
                    if (D_8019C044 == 1) {
                        D_8019C02C = 1;
                    }
                    D_8019C045 = 0;
                    D_8019C044 = 0;
                    D_8019CC1C = 0;
                    D_8019C032 = 1;
                    if (g_FxCommonEffectSetup[D_8019CC52].value23 != 0xFF) {
                        do {
                            nextIndex1 = g_FxCommonEffectSetup[D_8019CC52].value23;
                            D_8019CC52 = (s16) nextIndex1;
                            nextSetupOffset1 = nextIndex1 * 0x34;
                        } while (((FxCommonEffectSetupRecord *)((u8 *)g_FxCommonEffectSetup + nextSetupOffset1))->enabled == 0);
                        D_8019C018 = 1;
                        D_8019CC1C = ((FxCommonEffectSetupRecord *)((u8 *)g_FxCommonEffectSetup + nextSetupOffset1))->value0c;
                        func_80195994(nextIndex1, 4, 5, 0);
                        motionIndex1 = 0;
                        motionOffset1 = 0;
                        phaseBits = D_8019CC52;
                        D_8019C058 = 0x20;
                        motionBase1 = (s16)phaseBits;
                        do {
                            func_8018F55C(motionIndex1 << 8, motionBase1 + 0x40, func_8006EC6C((&D_801D0260), 2), &motionSample, &motionExtra);
                            motionIndex1 += 1;
                            *(s32 *)((u8 *)D_801EA268 + motionOffset1) = ((motionSample.x << 0x10) - *(s32 *)((u8 *)D_8019CAA8 + motionOffset1)) >> 5;
                            *(s32 *)((u8 *)D_801EA26C + motionOffset1) = ((motionSample.y << 0x10) - *(s32 *)((u8 *)D_8019CAAC + motionOffset1)) >> 5;
                            *(s32 *)((u8 *)D_801EA270 + motionOffset1) = ((motionSample.z << 0x10) - *(s32 *)((u8 *)D_8019CAB0 + motionOffset1)) >> 5;
                            motionOffset1 += 0x10;
                        } while (motionIndex1 < 0xAU);
                    }
                }
            }
        }
    }
    if ((D_8019C034 == 1) && (g_FxCommonMotionRemaining0 == 0) && (g_FxCommonMotionRemaining1 == 0)) {
        ((FxCommonVec3 *)&g_FxCommonMotionPosition1)->x = D_801EA58C->state[0];
        g_FxCommonMotionPosition1.y = D_801EA58C->state[1] - 0x12C;
        g_FxCommonMotionPosition1.z = D_801EA58C->state[2];
        ((FxCommonVec3 *)&g_FxCommonMotionPosition0)->x = D_801EA578->state[0];
        g_FxCommonMotionPosition0.y = D_801EA578->state[1];
        g_FxCommonMotionPosition0.z = D_801EA578->state[2];
    }
    if ((D_8019C034 == 2) && (g_FxCommonMotionRemaining0 == 0) && (g_FxCommonMotionRemaining1 == 0)) {
        ((FxCommonVec3 *)&g_FxCommonMotionPosition1)->x = D_801EA580->state[0];
        g_FxCommonMotionPosition1.y = D_801EA580->state[1];
        g_FxCommonMotionPosition1.z = D_801EA580->state[2];
        ((FxCommonVec3 *)&g_FxCommonMotionPosition0)->x = D_801EA578->state[0];
        g_FxCommonMotionPosition0.y = D_801EA578->state[1];
        g_FxCommonMotionPosition0.z = D_801EA578->state[2];
    }
    if (D_8019C046 == 1) {
        func_8018F55C(D_8019C04C, g_FxCommonEffectSetup[D_8019CC52].motionIndices[3], func_8006EC6C((&D_801D0260), 2), (FxCommonMotionVec *) &g_FxCommonMotionPosition1.x, &sampleExtra);
        func_8018F55C(D_8019C04C, g_FxCommonEffectSetup[D_8019CC52].motionIndices[2], func_8006EC6C((&D_801D0260), 2), (FxCommonMotionVec *) &g_FxCommonMotionPosition0.x, &sampleExtra);
        D_8019C04C += 0x80;
    }
    if (D_8019C034 == 0xA) {
        D_8019C030 += 1;
        func_8018F55C(D_8019C04C, 0x4E, func_8006EC6C((&D_801D0260), 2), (FxCommonMotionVec *) &g_FxCommonMotionPosition1.x, &sampleExtra);
        func_8018F55C(D_8019C04C, 0x4F, func_8006EC6C((&D_801D0260), 2), (FxCommonMotionVec *) &g_FxCommonMotionPosition0.x, &sampleExtra);
        D_8019C04C += 0x10;
        if ((s16) D_8019C030 == 0x4B) {
            D_8019C0C4 = func_801958D4(0xB, 0U);
            D_8019C0C8 = 0;
        }
        if ((s16)D_8019C030 == 0x11D) {
            func_801958D4(0xE, 1U);
        }
        if ((s16)D_8019C030 == 0x13B) {
            D_8019C0C4 = func_801958D4(0xC, 0U);
            D_8019C0C8 = 0;
        }
        if ((s16)D_8019C030 == 0x20D) {
            func_801958D4(0xE, 1U);
        }
        if ((s16)D_8019C030 == 0x22B) {
            D_8019C0C4 = func_801958D4(0xD, 0U);
            D_8019C0C8 = 0;
        }
        if ((s16)D_8019C030 == 0x2FD) {
            func_801958D4(0xE, 1U);
        }
        if ((s16)D_8019C030 == 0x31B) {
            D_8019C0C4 = func_801958D4(0xE, 0U);
            D_8019C0C8 = 0;
        }
        if ((s16)D_8019C030 == 0x3ED) {
            func_801958D4(0xE, 1U);
        }
        if ((s16)D_8019C030 == 0x40B) {
            D_8019C0C4 = func_801958D4(0xE, 1U);
        }
        if (D_8019C0C8 == 0) {
            D_8019C0CD = 0;
            D_8019C0CC = 0;
        }
        if (D_8019C0C8 == 0x18) {
            D_8019C0CD = 1;
        }
        if (D_8019C0C8 == 0xBA) {
            D_8019C0CD = 2;
        }
        D_8019C0C8 += 1;
        if ((D_8019C0CD == 0) && ((u8) D_8019C0CC < 0x78U)) {
            D_8019C0CC += 5;
        }
        if ((D_8019C0CD == 2) && ((u8) D_8019C0CC >= 0xBU)) {
            D_8019C0CC -= 5;
        }
        func_80038940(D_8019C0C4, D_8019C0CC, D_8019C0CC, D_8019C0CC);
        resourceTicks = D_8019C0BC + 1;
        D_8019C0BC = resourceTicks;
        if (resourceTicks == 0x4B1) {
            D_8019C02A = 1;
            D_8019C026 = 0x10;
            D_8019CA68 = D_8019CC52;
            func_800868AC(0x12C, 0);
            D_8019C0C0 = 1;
        }
        if (D_8019C0BC == 0x41A) {
            func_80086C5C(D_800B0DB5, 0xDC, 0);
        }
    }
    func_80195D3C();
    if (D_8019C018 != 0) {
        D_8019C018 = 0;
        if (D_8019C01C == 1) {
            func_8003746C(D_8019C028);
        }
        func_80038940(g_FxCommonEffectSetup[D_8019CC52].value25, 0x80, 0x80, 0x80);
        if (D_8019C034 != 0xA) {
            func_800375E0(g_FxCommonEffectSetup[D_8019CC52].value25, 3, &resourceResult);
        }
        D_8019C028 = (s16) g_FxCommonEffectSetup[D_8019CC52].value25;
        D_8019C01C = 1;
    }
    if (g_FxCommonMotionRemaining0 != 0) {
        remaining0 = g_FxCommonMotionRemaining0 - 1;
        g_FxCommonMotionRemaining0 = remaining0;
        accumX0 = g_FxCommonMotionAccum0.x + D_8019C08C;
        g_FxCommonMotionAccum0.x = accumX0;
        accumY0 = g_FxCommonMotionAccum0.y + D_8019C090;
        accumZ0 = g_FxCommonMotionAccum0.z + D_8019C094;
        g_FxCommonMotionAccum0.y = accumY0;
        g_FxCommonMotionAccum0.z = accumZ0;
        g_FxCommonMotionPosition0.x = g_FxCommonMotionOrigin0.x + (accumX0 >> (*(s16 *)&D_8019C056));
        g_FxCommonMotionPosition0.y = g_FxCommonMotionOrigin0.y + (accumY0 >> (*(s16 *)&D_8019C056));
        g_FxCommonMotionPosition0.z = g_FxCommonMotionOrigin0.z + (accumZ0 >> (*(s16 *)&D_8019C056));
        if ((remaining0 << 0x10) == 0) {
            g_FxCommonMotionPosition0.x = g_FxCommonMotionTarget0.x;
            g_FxCommonMotionPosition0.y = g_FxCommonMotionTarget0.y;
            g_FxCommonMotionPosition0.z = g_FxCommonMotionTarget0.z;
        }
    }
    if (g_FxCommonMotionRemaining1 != 0) {
        remaining1 = g_FxCommonMotionRemaining1 - 1;
        g_FxCommonMotionRemaining1 = remaining1;
        accumX1 = g_FxCommonMotionAccum1.x + D_8019C05C;
        g_FxCommonMotionAccum1.x = accumX1;
        accumY1 = g_FxCommonMotionAccum1.y + D_8019C060;
        accumZ1 = g_FxCommonMotionAccum1.z + D_8019C064;
        g_FxCommonMotionAccum1.y = accumY1;
        g_FxCommonMotionAccum1.z = accumZ1;
        g_FxCommonMotionPosition1.x = g_FxCommonMotionOrigin1.x + (accumX1 >> (*(s16 *)&D_8019C052));
        g_FxCommonMotionPosition1.y = g_FxCommonMotionOrigin1.y + (accumY1 >> (*(s16 *)&D_8019C052));
        g_FxCommonMotionPosition1.z = g_FxCommonMotionOrigin1.z + (accumZ1 >> (*(s16 *)&D_8019C052));
        if ((remaining1 << 0x10) == 0) {
            g_FxCommonMotionPosition1.x = g_FxCommonMotionTarget1.x;
            g_FxCommonMotionPosition1.y = g_FxCommonMotionTarget1.y;
            g_FxCommonMotionPosition1.z = g_FxCommonMotionTarget1.z;
        }
    }
}
