#ifndef PE1_ROOM_SHAKE_BURST_H
#define PE1_ROOM_SHAKE_BURST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_fx.h"
#include "pe1/room_orbit_trail.h"

/* Shake burst controller (room_m318): shakes the camera while it scatters
 * debris particles around its anchor, then flags the actor for the
 * scene script. */

typedef struct RoomShakeBurstPoint {
    s16 x, y, z;
} RoomShakeBurstPoint;

typedef struct RoomShakeBurstObject {
    u32 flags;                    /* 0x00 */
    u8 reserved04[0x14];
    u8 *status;                   /* 0x18 */
} RoomShakeBurstObject;

typedef struct RoomShakeBurstSlot {
    RoomShakeBurstObject *object;
} RoomShakeBurstSlot;

typedef struct RoomShakeBurstChannel {
    s32 reserved[2];
    RoomShakeBurstSlot *pool;     /* 0x08 */
} RoomShakeBurstChannel;

typedef RoomOrbitTrailPoolChannel RoomShakeBurstSparkChannel;

typedef struct RoomShakeBurstEntity {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomShakeBurstEntity;

typedef struct RoomShakeBurstEvent {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
    u8 reserved0E[8];
    s16 running;                  /* 0x16 */
} RoomShakeBurstEvent;

/* Sweep shake controller anchor: position plus the sweep angle handed to
 * each spawned particle. */
typedef struct RoomShakeSweepAnchor {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s32 angle;                    /* 0x08 */
} RoomShakeSweepAnchor;

/* Sweep particle: a scaled model that turns about its own axis while it
 * follows the sweep centre (state 0) or decays in place (state 1). */
typedef struct RoomShakeSweepParticle {
    s16 x, y, z;                  /* 0x00 */
    s16 state;                    /* 0x06 */
    s16 size;                     /* 0x08 */
    u16 angle;                    /* 0x0A */
    u16 timer;                    /* 0x0C */
    s16 stretch;                  /* 0x0E */
} RoomShakeSweepParticle;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomShakeSweepParticle, state) == 0x06,
                  room_shake_sweep_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomShakeSweepParticle, stretch) == 0x0E,
                  room_shake_sweep_stretch_offset);
PE1_STATIC_ASSERT(sizeof(RoomShakeSweepParticle) == 0x10,
                  room_shake_sweep_particle_size);

extern RoomShakeBurstEvent *D_800E2368;
extern RoomShakeBurstChannel *D_800F32D0;
extern RoomShakeBurstSparkChannel *D_800F33E0;
extern RoomShakeBurstEntity **D_8009D254;
extern void *D_800B0E64;
extern int D_800E27EC;
extern u16 D_800E11EA;
extern u16 D_800E2850[];

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomShakeBurstPoint *func_800CE610(void *pool);
extern int func_800D3FD8(void);
extern void func_8006DF50(void *channel, int id, int value, int volume, int pan);
extern int func_80071A54(void);
extern void func_80020D50(void);
/* Floor height record at 0x800942EC; reading it as a record keeps the
 * bounce compare after the particle's velocity store, as retail does. */
typedef RoomOrbitTrailFloor RoomShakeFloor;

extern RoomShakeFloor D_800942EC;
extern u8 *D_800F32D8;
extern void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
extern void func_80020DD0(void);

/* room_m318 particle callbacks and the sweep anchor. */
extern int func_80193FC4(int mode, RoomShakeBurstPoint *particle);
extern int func_8019326C(int mode, RoomShakeSweepParticle *particle);
extern GteShortVector D_80199904;

/* Sweep particle draw: model asset, matrix helpers and clut lookups. */
extern u16 D_800E11FA;
extern u16 GetClut(int x, int y);
extern int func_80077CF4(int angle);
extern int func_80077A64(int, int, int, int);
extern void func_80079754(void *rotation, RoomSpriteMatrix *matrix);
extern void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_800C6EF8(void *asset);
extern void func_800C7098(void *asset, int r, int g, int b);
extern void func_800C71E4(void *asset, RoomSpriteMatrix *matrix);
extern void func_800C6F4C(void *asset);

/* Flash burst controller (func_80195D20): its particle callback, colour
 * seed, glow track and the helpers it calls. */
extern int func_80195904(int mode, RoomOrbitTrailParticle *p);
extern RenderColor D_8018F210;
extern u8 D_80199770[];
extern int func_80077DC4(int angle);
extern void func_800D1AE0(RenderColor *color, int value, int step, int count);

/* Rising column controller (func_801955E4): the anchor rises from the
 * floor over the configured duration while sparks fall off it. */
typedef struct RoomRisingColumn {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 counter;                  /* 0x08 */
    s16 timer;                    /* 0x0A */
} RoomRisingColumn;

typedef struct RoomRisingColumnParams {
    s32 descend;                  /* 0x00 */
    s32 duration;                 /* 0x04 */
    s32 height;                   /* 0x08 */
    s32 offset;                   /* 0x0C */
} RoomRisingColumnParams;

extern GteShortVector D_80199924;
extern int func_80195190(int mode, RoomOrbitTrailParticle *p);
extern GteRotation D_8018F208;
extern u8 D_801996EC[];
extern u16 D_800E11E8;
extern u16 D_800E11F8;
extern int func_800D3F64(int sound, int handle);

/* Cross flash controller (func_80196820): an anchor with an orientation,
 * and particles that carry their own position, rotation and draw kind. */
typedef struct RoomCrossFlash {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    GteRotation rotation;         /* 0x08 */
} RoomCrossFlash;

typedef struct RoomCrossFlashParticle {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    GteRotation rotation;         /* 0x08: flags selects the flash draw */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomCrossFlashParticle;

extern GteShortVector D_80199930;
extern int func_8019646C(int mode, RoomCrossFlashParticle *p);
extern RenderColor D_8018F1FC;
extern u8 D_80199798[];

/* Ember burst particle (func_801944E8): spin template, colour seed and
 * glow track. */
extern GteRotation D_8018F1F4;
extern u8 D_80199674[];

#endif
