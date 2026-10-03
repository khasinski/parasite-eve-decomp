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

/* Double-buffered frame: packet buffer and ordering table, then the
 * DRAWENV and DISPENV handed to PutDrawEnv / PutDispEnv. */
typedef struct FxCommonFrame {
    FxCommonBuffer buffer;
    u8 drawEnv[0x5C];   /* 0x08 */
    u8 dispEnv[0x14];   /* 0x64 */
} FxCommonFrame;
typedef struct FxCommonRect {
    s16 x, y, w, h;
} FxCommonRect;
extern FxCommonFrame g_FxCommonFrames[2] __asm__("D_8019C1F8");
extern s32 D_8009CDDC;
extern u8 D_8019C00E;
extern s32 D_8019CC14;
extern u32 D_8009D280;
extern s8 D_800B0DB2;
extern s8 D_800B0DB4;
extern u8 D_800BCE80[];

void func_80086C5C(int channel, int value, int mode);
void func_80196498(void);
void func_80191DE8(int enabled);
s32 func_80073A44(s32 value);
void func_80071A64(int seed);
int func_80071A54(void);
void func_8003EB04(void);
void func_80074D28(s32 value);
void func_8006A25C(void);
void func_801942FC(void);
void func_8018F05C(void);
void func_8018F92C(void *motion);
s16 func_80194108(s16 value);
void func_80192740(void);
void func_80192800(void);
void func_80193478(void);
int func_80191E30(int id, s32 *state);
void func_80191EFC(int handle, s32 *state);
void func_80037870(void);
void func_80074DC0(s32 value);
void func_80193AB0(void);
void func_80074A44(s32 value);
void func_80075424(void *draw_env);
void func_800755F0(void *entry);
void func_800753B4(void *ordering_table);
void func_80192030(void);
void func_80074F44(void *rect, s32 x, s32 y, s32 mode);
void func_80086FF8(void);
void func_80087024(void);
void func_80038D48(void);
s16 func_801958D4(s16 resourceId, u8 action);
void func_80195BC8(FxCommonShortVec3 *a, FxCommonShortVec3 *b, int bi, int ai);
void func_80195D3C(void);

PE1_STATIC_ASSERT(sizeof(FxCommonFrame) == 0x78, fx_common_frame_size);
PE1_STATIC_ASSERT(sizeof(FxCommonVec3) == 12, fx_motion_position_size);
PE1_STATIC_ASSERT(sizeof(FxCommonShortVec3) == 6, fx_motion_endpoint_size);

#endif
