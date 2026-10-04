#ifndef PE1_ROOM_M005_H
#define PE1_ROOM_M005_H

#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Drifting sprite drawn by the room_m005 effect callback. */
typedef struct RoomM005DriftingSpriteState {
    unsigned short x;             /* 0x00 */
    unsigned short y;             /* 0x02 */
    unsigned short z;             /* 0x04 */
    short pad06;                  /* 0x06 */
    unsigned short vx;            /* 0x08 */
    unsigned short vy;            /* 0x0A */
    unsigned short vz;            /* 0x0C */
} RoomM005DriftingSpriteState;

/* Orbiter effect controller state and the event flag it watches. */
typedef struct RoomM005OrbiterState {
    u8 reserved[8];
    s16 armed;                    /* 0x08 */
    u16 frame;                    /* 0x0A */
} RoomM005OrbiterState;

typedef struct RoomM005OrbiterChild {
    s16 x, y, z;
    s16 flag;                     /* 0x06 */
    s16 seed;                     /* 0x08 */
    s16 radius;                   /* 0x0A */
    s16 state;                    /* 0x0C */
    s16 timer;                    /* 0x0E */
} RoomM005OrbiterChild;

typedef struct RoomM005Channel {
    s32 reserved[2];
    char *pool;
} RoomM005Channel;

typedef struct RoomM005EventState {
    u8 reserved[0x12];
    s16 triggered;                /* 0x12 */
} RoomM005EventState;

typedef struct RoomM005Seed8 {
    u8 bytes[8];
} RoomM005Seed8;

extern RoomM005Seed8 D_8018EFF4;
extern u8 D_80190B84[];
extern u8 D_80190B8C[];
extern void *D_800B0E64;
extern RoomM005Channel *D_800F32D0, *D_800F33E0;
extern RoomM005EventState *D_800E2368;
extern volatile u16 D_800E11E8;
extern volatile s16 D_800F336E, D_800F3372, D_800F3374, D_800F3370;
extern volatile s16 D_800F3376, D_800F3378;
extern int func_800D3FD8(void);
extern void func_8006DF50(void *channel, int id, int value, int volume, int pan);
extern int func_800CE560(void *, int, int, void *);
extern void func_800CE8F0(void *, int, void *, void *);
extern RoomM005OrbiterChild *func_800CE610(void *);
extern int func_80071A54(void);
extern void RoomM005_FxOrbiter_8018F018(void);
int func_8018F330(int mode, RoomM005OrbiterState *state);

extern RenderColor D_8018F00C;
extern unsigned short D_80190BA0;
extern unsigned short D_80190BA2;
extern unsigned short D_80190BA4;
extern GteShortVector D_80190BA8;

int func_80077DC4(int angle);
int func_80077CF4(int angle);

/* Cross flare (func_8018F614): two joint templates and the flash track. */
extern RoomM005Seed8 D_8018EFFC;
extern RoomM005Seed8 D_8018F004;
extern u8 D_80190AF4[];
extern u16 D_800E11EA;
extern u16 D_800E11FA;
int func_80077AA4(int, int);
int func_8018FB84(int mode, RoomM005DriftingSpriteState *state);

#endif
