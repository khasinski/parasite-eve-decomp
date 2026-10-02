#ifndef PE1_ROOM_SOUND_BURST_H
#define PE1_ROOM_SOUND_BURST_H

#include "common.h"

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
} RoomSoundBurstEventState;

typedef struct RoomSoundBurstState {
    u8 reserved[0xA];
    u16 frame;                    /* 0x0A */
    s16 soundHandle;              /* 0x0C */
} RoomSoundBurstState;

typedef struct RoomSoundBurstParticle {
    s16 x, y, z;
    s16 seed;                     /* 0x06 */
    s16 reserved8[3];
    s16 angle;                    /* 0x0E */
    s16 height;                   /* 0x10 */
    s16 state;                    /* 0x12 */
    s16 timer;                    /* 0x14 */
    s16 phase;                    /* 0x16 */
} RoomSoundBurstParticle;

PE1_STATIC_ASSERT(sizeof(RoomSoundBurstParticle) == 0x18,
                  room_sound_burst_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSoundBurstState, soundHandle) == 0xC,
                  room_sound_burst_state_sound_handle);

extern RoomSoundBurstChannel *D_800F32D0, *D_800F33E0;
extern RoomSoundBurstEventState *D_800E2368;
extern s32 D_800E27EC;
extern volatile u16 D_800E11E8;
extern u16 D_800E2850[];
extern u16 D_800F336C;
extern volatile s16 D_800F336E, D_800F3372, D_800F3374, D_800F3370;
extern s16 D_800F336A;
extern volatile s16 D_800F3368, D_800F3376, D_800F3378;
extern int func_800CE560(void *, int, int, void *);
extern void func_800CE8F0(void *, int, void *, void *);
extern void func_800CE9D4(void *, int, void *);
extern RoomSoundBurstParticle *func_800CE610(void *);
extern int rsin(int);
extern int func_800D3FD8(void);
extern int func_800D3F64(int, int);
extern void func_800866A4(int, int);
extern int func_80071A54(void);

#endif
