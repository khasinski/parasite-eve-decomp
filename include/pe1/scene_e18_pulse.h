#ifndef PE1_SCENE_E18_PULSE_H
#define PE1_SCENE_E18_PULSE_H

#include "common.h"

/* Emitter and callback views of the same eight-byte scene_e18 pool slot. */
typedef struct SceneE18PulseRing {
    s16 size;                     /* 0x00 */
    s16 shrink;                   /* 0x02 */
    s16 radius;                   /* 0x04 */
    u8 active;                    /* 0x06 */
    u8 last;                      /* 0x07 */
} SceneE18PulseRing;

typedef struct SceneE18PulseParticle {
    s16 scale;                    /* 0x00 */
    s16 duration;                 /* 0x02 */
    u8 reserved04[2];             /* 0x04 */
    u8 active;                    /* 0x06 */
    u8 final_stage;               /* 0x07 */
} SceneE18PulseParticle;

typedef union SceneE18PulseRecord {
    SceneE18PulseRing ring;
    SceneE18PulseParticle particle;
} SceneE18PulseRecord;

PE1_STATIC_ASSERT(sizeof(SceneE18PulseRing) == 8, scene_e18_pulse_ring_size);
PE1_STATIC_ASSERT(sizeof(SceneE18PulseParticle) == 8,
                  scene_e18_pulse_particle_size);
PE1_STATIC_ASSERT(sizeof(SceneE18PulseRecord) == 8,
                  scene_e18_pulse_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE18PulseRing, active) == 6,
                  scene_e18_pulse_ring_active_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE18PulseRing, last) == 7,
                  scene_e18_pulse_ring_last_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE18PulseParticle, active) == 6,
                  scene_e18_pulse_particle_active_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE18PulseParticle, final_stage) == 7,
                  scene_e18_pulse_particle_final_stage_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE18PulseRecord, particle) == 0,
                  scene_e18_pulse_record_particle_offset);

#endif
