#ifndef PE1_BOOT_DISPLAY_H
#define PE1_BOOT_DISPLAY_H

#include "common.h"

typedef struct {
    s32 tag;
    s32 code;
} BootDisplayDrawModePrim;

typedef struct {
    s32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    u16 x0;
    u16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    u16 w;
    u16 h;
} BootDisplaySpritePrim;

typedef struct {
    BootDisplayDrawModePrim draw_mode;
    BootDisplaySpritePrim sprite;
} BootDisplaySpritePacket;

extern char *g_BootDisplayOrderingTable;
extern u16 g_BootDisplayTPage;
extern u16 g_BootDisplayClut;
extern char *g_BootDisplayPrimitiveCursor;
extern s32 g_BootDisplayTextScrollX;
extern s32 g_BootDisplayTextScrollStepX;
extern s32 g_BootDisplayTransition;
extern s32 g_BootDisplayBufferIndex;
extern s16 g_BootDisplayStateTimer;
extern s16 *D_80172C90;
extern s16 *D_80172C94;
extern s32 D_80172C98;
extern s32 D_80172C9C;
extern u16 D_80093166;
extern u16 D_80093168;
extern char *D_80011610;
extern char D_8012682C[];

void func_80077AC4(void *ordering_table, void *primitive);
void func_80077B04(void *primitive, s32 code);
void func_80077C84(void *primitive, s32 x, s32 y, u16 tpage);
void func_80077CB4(void *primitive, void *next);
void func_80074A44(s32 value);
void func_80074BB8(s32 value);
void func_80074D28(s32 value);
void func_80074DC0(s32 value);
void func_80074F44(void *rect, s32 x, s32 y, s32 mode);
s32 func_800718D0(void *resource);
s32 func_800719C4(void *resource);
s16 *func_80071964(void *resource);
s16 *func_80071944(void *resource);
s32 func_80077A64(s32, s32, s32, s32);
s32 func_80077AA4(s32, s32);
void func_8010BE3C(s32 value);
void func_8010BD4C(void *address);

/* Callable entry points inside neighboring boot-display code. */
s32 func_80123B1C(u8 color);
void func_8012462C(void *primitive, u16 x, u16 y, s32 u, s32 v);

#endif
