#ifndef FX_COMMON_MOTION_H
#define FX_COMMON_MOTION_H

#include "fx_common_setup.h"

extern volatile FxCommonVec3 g_FxCommonMotionPosition1 __asm__("D_8019C330");
extern volatile FxCommonVec3 g_FxCommonMotionPosition0 __asm__("D_8019C810");
extern volatile FxCommonVec3 g_FxCommonMotionAccum1 __asm__("D_8019C06C");
extern volatile FxCommonVec3 g_FxCommonMotionAccum0 __asm__("D_8019C09C");
extern FxCommonShortVec3 g_FxCommonMotionOrigin1 __asm__("D_8019C07C");
extern FxCommonShortVec3 g_FxCommonMotionTarget1 __asm__("D_8019C084");
extern FxCommonShortVec3 g_FxCommonMotionOrigin0 __asm__("D_8019C0AC");
extern FxCommonShortVec3 g_FxCommonMotionTarget0 __asm__("D_8019C0B4");
/* Ordinary counter views; legacy callers use volatile scheduling declarations. */
extern s16 g_FxCommonMotionRemaining0 __asm__("D_8019C054");
extern s16 g_FxCommonMotionRemaining1 __asm__("D_8019C050");
extern s32 D_8009D1F4;
extern s32 D_8009D26C;
extern s8 D_800B0DB5;
extern void *volatile D_800B0E08[1];
extern s32 D_8019C018;
extern s32 D_8019C01C;
extern s16 D_8019C024;
extern s16 D_8019C026;
extern s16 D_8019C028;
extern s16 D_8019C02A;
extern u16 D_8019C030;
extern s16 D_8019C032;
extern s32 D_8019C038;
extern s32 D_8019C03C;
extern s32 D_8019C04C;
extern s32 D_8019C0BC;
extern s32 D_8019C0C0;
extern s16 D_8019C0C4;
extern s32 D_8019C0C8;
extern u8 D_8019C0CC;
extern u8 D_8019C0CD;
extern s16 D_8019C13C;
extern s16 D_8019C13E;
extern s32 D_8019CA68;

void func_80086C5C(int channel, int value, int mode);
s16 func_801958D4(s16 resourceId, u8 action);
void func_80195BC8(FxCommonShortVec3 *a, FxCommonShortVec3 *b, int bi, int ai);
void func_80195D3C(void);

PE1_STATIC_ASSERT(sizeof(FxCommonVec3) == 12, fx_motion_position_size);
PE1_STATIC_ASSERT(sizeof(FxCommonShortVec3) == 6, fx_motion_endpoint_size);

#endif
