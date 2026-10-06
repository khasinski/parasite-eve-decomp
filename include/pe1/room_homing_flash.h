#ifndef PE1_ROOM_HOMING_FLASH_H
#define PE1_ROOM_HOMING_FLASH_H

#include "common.h"
#include "pe1/room_sound_burst.h"

/* The homing flash (src/overlays/room_lib/RoomEffect_HomingFlash.c): a
 * flash controller anchored to the actor and the homing sparks it
 * releases. */

typedef struct RoomFlashSpriteState {
    s16 x, y, z, reserved06;      /* 0x00: drawn position */
    s16 ax;                       /* 0x08: anchor */
    u16 ay;                       /* 0x0A */
    s16 az;                       /* 0x0C */
    s16 reserved0E;
    s16 state;                    /* 0x10 */
    u16 frame;                    /* 0x12 */
    s16 attachment;               /* 0x14 */
    s16 soundHandle;              /* 0x16 */
} RoomFlashSpriteState;

typedef struct RoomFlashSpriteSpawn {
    s32 reserved[2];
    s32 lift;                     /* 0x08 */
} RoomFlashSpriteSpawn;

typedef struct RoomFlashSpriteChild {
    s16 x, y, z, reserved06;
    s16 wx, wy, wz, reserved0E;
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
    s16 soundHandle;              /* 0x14 */
} RoomFlashSpriteChild;

/* Per-room data: the colour ramp the controller blends through. */
extern u8 g_RoomFlashColorRamp[];

/* The sprite parameter block at 0x800F3368. */
extern RoomSoundBurstParams D_800F3368;
extern u16 D_800E11EC;

int RoomEffect_HomingSpark(int mode, RoomHomingSpark *spark,
                           RoomHomingSparkParams *params);
int RoomEffect_FlashSpriteController(int mode, RoomFlashSpriteState *state,
                                     RoomFlashSpriteSpawn *spawn);

#endif
