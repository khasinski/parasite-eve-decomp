#ifndef PE1_ROOM_SOUND_BURST_H
#define PE1_ROOM_SOUND_BURST_H

#include "common.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_transform_point.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/room_floor.h"

/* Room effect controllers that play a sound, then emit one particle every
 * third frame along an angle that sweeps half a turn over seventy frames. */

typedef struct RoomSoundBurstTemplate8 {
    s32 word[2];
} __attribute__((packed)) RoomSoundBurstTemplate8;

typedef struct RoomSoundBurstChannel {
    s32 reserved[2];
    char *pool;
} RoomSoundBurstChannel;

typedef struct RoomSoundBurstNode {
    u8 reserved[0x18];
    u8 *state;
} RoomSoundBurstNode;

typedef struct RoomSoundBurstEventState {
    u8 reserved[0xD];
    u8 active;
    u8 reserved0E[4];
    s16 phase;                    /* 0x12 */
} RoomSoundBurstEventState;

typedef struct RoomSoundBurstState {
    u8 reserved[0xA];
    u16 frame;                    /* 0x0A */
    s16 soundHandle;              /* 0x0C */
} RoomSoundBurstState;

typedef struct RoomSoundBurstParticle {
    s16 x, y, z;                  /* 0x00: anchor */
    s16 tilt;                     /* 0x06: random angle for the size cosine */
    s16 wx, wy, wz;               /* 0x08: world position */
    s16 spread;                   /* 0x0E: controller angle, scales the sprite */
    s16 height;                   /* 0x10 */
    u16 phase;                    /* 0x12: sine phase, stepped per frame */
    u16 swing;                    /* 0x14: swing angle, stepped per frame */
    u16 amplitude;                /* 0x16: swing amplitude, stepped per frame */
} RoomSoundBurstParticle;

typedef struct RoomSoundBurstColor {
    u8 r, g, b, code;
} RoomSoundBurstColor;

/* Battle actor record reached through the active battle entity. */
typedef struct RoomSoundBurstActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomSoundBurstActor;

typedef struct RoomSoundBurstBattleEntity {
    RoomSoundBurstActor *actor;
} RoomSoundBurstBattleEntity;

PE1_STATIC_ASSERT(sizeof(RoomSoundBurstParticle) == 0x18,
                  room_sound_burst_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSoundBurstState, soundHandle) == 0xC,
                  room_sound_burst_state_sound_handle);

extern void Akao_Cmd_21(int, int);
extern int rand(void);
extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern void FieldFx_OffsetPointByAngles(void *, void *, int, void *, int, int);
extern void LoadAverageShort12(void *, void *, int, int, void *);
extern void FieldFx_DrawGlowLine(void *, void *, void *, void *, int, int, int);

/* Homing spark: anchor, heading vector, state, timer and sound handle. */
typedef struct RoomHomingSpark {
    s16 x, y, z, reserved06;
    s16 hx, hy, hz, reserved0E;   /* 0x08: heading */
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
    s16 soundHandle;              /* 0x14 */
} RoomHomingSpark;

typedef struct RoomHomingSparkParams {
    s16 distance;                 /* 0x00 */
    s16 reserved02;
    u16 speed;                    /* 0x04 */
} RoomHomingSparkParams;

PE1_STATIC_ASSERT(sizeof(RoomHomingSpark) == 0x16, room_homing_spark_size);

#endif
