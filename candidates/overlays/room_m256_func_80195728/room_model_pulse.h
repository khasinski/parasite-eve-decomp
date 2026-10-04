#ifndef PE1_ROOM_MODEL_PULSE_H
#define PE1_ROOM_MODEL_PULSE_H

#include "pe1/room_spark.h"

/* room_m256 pulsing model effect: one controller (func_80195728) that seeds
 * sixteen ring particles on frame 1, draws a ring sprite at the slot-2
 * position and, from frame 4 on, a model scaled by cos and brightened by sin.
 * The ring particles (func_8019552C) read the shared state below. */

typedef struct RoomModelPulseParticle {
    u16 frame;                    /* 0x00 */
    u16 offset;                   /* 0x02 */
    s16 scale;                    /* 0x04 */
} RoomModelPulseParticle;

typedef struct RoomModelPulseMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} RoomModelPulseMatrix;

typedef struct RoomModelPulseScale {
    s32 x, y, z, reserved0C;
} RoomModelPulseScale;

typedef struct RoomModelPulseAnchor {
    s16 x, y, z;
} RoomModelPulseAnchor;

extern GteRotation D_8018F258;          /* ring placement rotation seed */
extern u8 D_80195E8C[];                 /* colour ramp table */
extern s16 D_80196094;                  /* ring brightness */
extern GteShortVector D_80196098;       /* ring/model spin (slot-2 output) */
extern RoomModelPulseAnchor D_801960A0; /* ring centre */
extern void *D_801960A8;                /* model asset */

extern u16 D_800E11E8;
extern u16 D_800E11FA;
extern void *func_8006E498(void *base, u32 key);
extern void func_800C6D5C(void *asset, int x, int y);
extern s32 func_80077A64(s32, s32, s32, s32);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_80079754(void *rotation, void *matrix);
extern void func_80078CC4(void *matrix, void *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C6FA0(void *asset, int brightness);
extern void func_800C71E4(void *asset, void *matrix);
extern void func_800C6F4C(void *asset);
extern RoomModelPulseParticle *func_800CE610_pulse(void *pool) __asm__("func_800CE610");
extern int func_8019552C(int mode, RoomModelPulseParticle *particle);
int func_80195728(int mode, s16 *state);

#endif /* PE1_ROOM_MODEL_PULSE_H */
