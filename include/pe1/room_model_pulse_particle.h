#ifndef PE1_ROOM_MODEL_PULSE_PARTICLE_H
#define PE1_ROOM_MODEL_PULSE_PARTICLE_H

#include "common.h"

/* room_m256 pulsing model: one ring particle. func_8019552C advances and
 * draws it; the model pulse controller (func_80195728) seeds sixteen. */
typedef struct RoomModelPulseParticle {
    u16 frame;                    /* 0x00 */
    u16 offset;                   /* 0x02 */
    s16 scale;                    /* 0x04 */
} RoomModelPulseParticle;

int func_8019552C(int mode, RoomModelPulseParticle *particle);

#endif /* PE1_ROOM_MODEL_PULSE_PARTICLE_H */
