#ifndef MENU_MEMCARD_GLINT_H
#define MENU_MEMCARD_GLINT_H

#include "pe1/room_spark.h"
#include "pe1/gte.h"

/* Scene object reached through the primary channel pool. */
typedef struct MemcardGlintAnchor {
    u8 reserved[0x268];
    s16 x, y, z;                  /* 0x268 */
} MemcardGlintAnchor;

typedef struct MemcardGlintBurst {
    s16 x, y, z;
} MemcardGlintBurst;

/* Spinning particle: a position (state 0) or the spin rates that drive
 * its rotation (states 1 and 2). */
typedef struct MemcardSpinParticle {
    s16 x, y, z, reserved06;      /* 0x00 */
    GteRotation rotation;         /* 0x08 */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} MemcardSpinParticle;

typedef struct MemcardSpinBurst {
    s16 x, y, z, reserved06;      /* 0x00 */
    GteRotation rotation;         /* 0x08 */
} MemcardSpinBurst;

typedef struct MemcardModelMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} MemcardModelMatrix;

typedef struct MemcardModelScale {
    s32 x, y, z, reserved0C;
} MemcardModelScale;

extern RenderColor D_801ED844, D_801ED848, D_801ED84C;
extern GteRotation D_801ED7FC, D_801ED804;
extern RenderColor D_801ED80C, D_801ED810, D_801ED814;
extern u8 D_801F1BB0[];
extern u8 D_801F1C28[];
extern u8 D_801F1CD8[];
extern GteShortVector D_801F1F28;
extern s16 D_801F1F3A;
extern u8 *D_800E22D4;
extern void *D_8009D254;
extern s32 func_80077A64(s32, s32, s32, s32);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_80079754(void *rotation, void *matrix);
extern void func_80078CC4(void *matrix, void *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C6FA0(void *asset, int brightness);
extern void func_800C71E4(void *asset, void *matrix);
extern void func_800C6F4C(void *asset);
extern void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
extern void func_8006DDCC(int sound, int mode, int x, int y, int z);
extern u8 D_801F1CB0[];
extern void func_800CF3AC(void *track, void *color, int time);
extern void func_800D1AE0(RenderColor *color, int intensity, int step, int count);

int Memcard_FadingGlintParticle(int mode, RoomDampedSpark *spark);
int Memcard_GlintBurstController(int mode, MemcardGlintBurst *burst);
int Memcard_SpinRingParticle(int mode, MemcardSpinParticle *p);
int Memcard_SpinBurstController(int mode, MemcardSpinBurst *burst);
int Memcard_DriftGlowParticle(int mode, RoomDampedSpark *spark);
int Memcard_RisingEmberParticle(int mode, RoomDampedSpark *spark);
int Memcard_RingBurstController(int mode, RoomDampedSpark *burst);

GteShortVector *Memcard_GetGlintOriginA(void);
GteShortVector *Memcard_GetGlintOriginB(void);
GteShortVector *Memcard_GetGlintOriginC(void);
GteShortVector *Memcard_GetGlintOriginD(void);
GteShortVector *Memcard_GetGlintOriginE(void);
GteShortVector *Memcard_GetGlintOriginF(void);
int Memcard_ConsumeCrossFlashFlag(int mode);

#endif
