#ifndef PE1_ROOM_FALLING_BURST_H
#define PE1_ROOM_FALLING_BURST_H

#include "common.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "pe1/room_module.h"

/* The falling sprite burst linked by ten hospital and Chrysler rooms
 * (src/overlays/room_lib/RoomFx_FallingSpriteBurst.c): a sprite and its
 * floor shadow fall until they reach the floor, then spawn an impact
 * shimmer, a ground pulse and six particles. */

/* Effect control pair: state byte 1 is set to 2 when the effect is done. */
typedef struct RoomFallingBurstControl {
    u8 pad0;
    u8 state;                     /* 0x01 */
    s16 endTick;                  /* 0x02 */
} RoomFallingBurstControl;

/* Shared scene clock returned by func_800C2B50. */
typedef struct RoomFallingBurstClock {
    u8 pad0[0x8];
    s16 hit;                      /* 0x08 */
    s16 step;                     /* 0x0A */
    s16 tick;                     /* 0x0C */
    u8 padE[0x2];
    u8 renderOwner;               /* 0x10 */
} RoomFallingBurstClock;

/* Parameter block of the floor decals: the ground pulse uses the scale and
 * depth at 0x08 (as RoomUniformSpriteFxParams), the impact shimmer the
 * ones at 0x10 and its alpha (as RoomSpriteFxParams). */
typedef struct RoomFallingBurstDecal {
    s16 x, y, z;                  /* 0x00 */
    s16 pad6;
    s16 pulseScale;               /* 0x08 */
    s16 pulseDepth;               /* 0x0A */
    s16 hC;                       /* 0x0C */
    u8 padE[0x2];
    s16 shimmerScale;             /* 0x10 */
    s16 shimmerDepth;             /* 0x12 */
    u8 pad14;
    u8 shimmerAlpha;              /* 0x15 */
    u8 shimmerPhase;              /* 0x16 */
} RoomFallingBurstDecal;

typedef struct RoomFallingBurstVec {
    u16 x, y, z;
    s16 pad6;
} RoomFallingBurstVec;

/* The six impact particles. */
typedef struct RoomFallingBurstParticles {
    RoomSpriteMatrix matrix;          /* 0x00 */
    RoomFallingBurstVec position[6];  /* 0x20 */
    RoomFallingBurstVec velocity[6];  /* 0x50 */
    u8 pad80[0xC];
    u16 depth[6];                     /* 0x8C */
    s16 active[6];                    /* 0x98 */
    s16 liveCount;                    /* 0xA4 */
} RoomFallingBurstParticles;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFallingBurstParticles, liveCount) == 0xA4,
                  room_falling_burst_particles_live);

/* The frame counter at 0x800942EC, read signed as the floor height. */
typedef struct RoomFallingBurstFloor {
    s16 height;
} RoomFallingBurstFloor;

extern RoomFallingBurstFloor g_FrameCount16;

/* Per-room data: the five sprite records. When it lands the falling sprite
 * spawns from the room module's init list and spawn layout
 * (pe1/room_module.h). */
extern RoomFxSpritePacket g_RoomFallingBurstShadow;
extern RoomFxSpritePacket g_RoomFallingBurstShimmer;
extern RoomFxSpritePacket g_RoomFallingBurstSprite;
extern RoomFxSpritePacket g_RoomFallingBurstPulse;
extern RoomFxSpritePacket g_RoomFallingBurstParticle;

void *func_8006DC18(int type);
int *func_800C2B10(int index);
int *func_800C2B28(int index);
void *func_800C2B90(void *entity, int kind, void *script, void *data);
int func_800C2B68(void);
void func_800C6800(void *entity, int effectId, void *state);
void func_800C6C18(int entity);
int func_800C6B90(void *position, int radius);
void func_800C2EAC(u8 owner);
void func_800C2FF0(int width, int height);
void func_800C3098(int depth);
void func_800C3238(int mode);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);
void func_80071A44(void *dst, int value, int size);
int func_80071A54(void);
void ApplyMatrixSV(void *transform, RoomFxSeed8 *seed, u16 *out);
void RotMatrix(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void ScaleMatrix(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);

#endif
