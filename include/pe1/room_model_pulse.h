#ifndef PE1_ROOM_MODEL_PULSE_H
#define PE1_ROOM_MODEL_PULSE_H

#include "pe1/room_spark.h"
#include "pe1/room_model_pulse_particle.h"

/* room_m256 pulsing model effect: one controller (func_80195728) that seeds
 * sixteen ring particles on frame 1, draws a ring sprite at the slot-2
 * position and, from frame 4 on, a model scaled by cos and brightened by sin.
 * The ring particles (func_8019552C) read the shared state below. */

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
int func_80195728(int mode, s16 *state);

#endif /* PE1_ROOM_MODEL_PULSE_H */
