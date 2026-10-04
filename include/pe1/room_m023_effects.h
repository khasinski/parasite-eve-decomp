#ifndef PE1_ROOM_M023_EFFECTS_H
#define PE1_ROOM_M023_EFFECTS_H

#include "common.h"
#include "pe1/render_object.h"

/* Room m023 controller that scatters particles around a model joint,
 * either at the joint itself or at a random point of a wide band. */

typedef struct RoomM023EventState {
    u8 reserved[0x12];
    s16 variant;                  /* 0x12 */
    s16 interval;                 /* 0x14 */
} RoomM023EventState;

typedef struct RoomM023Channel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} RoomM023Channel;

typedef struct RoomM023ScatterState {
    u8 reserved[8];
    s16 unused08;                 /* 0x08 */
    s16 unused0A;                 /* 0x0A */
    s16 timer;                    /* 0x0C */
    s16 joint;                    /* 0x0E */
} RoomM023ScatterState;

typedef struct RoomM023Particle {
    s16 x, y, z;
    s16 attached;                 /* 0x06 */
    s16 vx, vy, vz;               /* 0x08 */
} RoomM023Particle;

/* The drift-pulse callback reads the scatter particle's attached word as
 * its phase and shares the same velocity halfwords. */
typedef struct RoomM023DriftPulseCallbackView {
    u16 x, y, z;                  /* 0x00 */
    s16 phase;                    /* 0x06 */
    u16 vx, vy, vz;               /* 0x08 */
} RoomM023DriftPulseCallbackView;

typedef union RoomM023ParticleRecord {
    RoomM023Particle scatter;
    RoomM023DriftPulseCallbackView driftPulse;
} RoomM023ParticleRecord;

PE1_STATIC_ASSERT(sizeof(RoomM023ParticleRecord) == 0x0E,
                  room_m023_particle_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM023ParticleRecord, driftPulse.phase) ==
                      PE1_OFFSETOF(RoomM023ParticleRecord, scatter.attached),
                  room_m023_particle_phase_view);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM023ParticleRecord, driftPulse.vx) ==
                      PE1_OFFSETOF(RoomM023ParticleRecord, scatter.vx),
                  room_m023_particle_velocity_view);

typedef struct RoomM023Template {
    s32 word[2];
} __attribute__((packed)) RoomM023Template;

extern RoomM023EventState *D_800E2368;
extern RoomM023Channel *D_800F32D0, *D_800F33E0;
extern RoomM023Template D_8018EFF4;
extern GteShortVector D_80190758;
extern u16 D_800E11E4[];

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern void func_800CE8F0(void *pool, int index, void *template, void *position);
extern RoomM023Particle *func_800CE610(void *pool);
extern int func_80071A54(void);
extern int func_8018F004(int mode,
                         RoomM023DriftPulseCallbackView *particle);

/* Joint glow: a ring burst, then a growing and pulsing glow sprite pair
 * drawn at a model joint. */
typedef struct RoomM023JointGlow {
    s16 joint;
    s16 frame;
    s16 state;
} RoomM023JointGlow;

extern u16 D_800E11EA;
extern RenderColor D_8018EFFC;
extern RenderColor D_8018F000;
extern int func_800D3FD8(void);
extern int func_800D3F64(int sound, int handle);
extern u16 GetClut(int x, int y);
int func_8018F710(int mode, RoomM023JointGlow *glow);

#endif
