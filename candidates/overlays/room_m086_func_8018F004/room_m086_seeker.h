#ifndef PE1_ROOM_M086_SEEKER_H
#define PE1_ROOM_M086_SEEKER_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/field_model_draw.h"

/* room_m086 falling seeker (func_8018F004): a turning model that flies
 * along its heading until it lands, bursts on the player (flagging the
 * battle actor), then rises and fades or bounces as a flaring spark. */
typedef struct RoomM086Seeker {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector heading;
    /* 0x10 */ s16 timer;
    /* 0x12 */ s16 state;
} RoomM086Seeker;

typedef struct RoomM086SeekerParams {
    /* 0x00 */ u32 size;
    /* 0x04 */ s16 distance;
} RoomM086SeekerParams;

typedef struct RoomM086Object {
    u32 flags;
} RoomM086Object;

typedef struct RoomM086Pool {
    RoomM086Object *object;
} RoomM086Pool;

typedef struct RoomM086Channel {
    s32 reserved[2];
    RoomM086Pool *pool;
} RoomM086Channel;

typedef struct RoomM086Actor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM086Actor;

typedef struct RoomM086BattleEntity {
    RoomM086Actor *actor;
} RoomM086BattleEntity;

typedef struct RoomM086EventState {
    u8 reserved[0xD];
    u8 active;
} RoomM086EventState;

typedef struct RoomM086FloorLevel {
    s16 count;
} RoomM086FloorLevel;

extern RoomM086FloorLevel D_800942EC;

/* The frame counter read as a one-field record: the in-struct load cannot
 * pass the parameter block stores around it, which is where retail keeps
 * it in the spark flare draw. */
typedef struct RoomM086FrameCounter {
    int value;
} RoomM086FrameCounter;

extern RoomM086FrameCounter D_800E27EC;

/* The sprite parameter block at 0x800F3368 (RenderEffectParameters). */
typedef struct RoomM086EffectParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomM086EffectParams;

extern RoomM086EffectParams D_800F3368;
extern u16 D_800E11E4[];
extern u16 D_800E1204[];
extern u16 D_800E2850[];
extern int D_800F3428;
void func_800CFB7C(GteShortVector *angles, s16 distance, GteShortVector *out);
void func_800CEE20(GteShortVector *position, GteRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, void *color);
extern RoomM086Channel *D_800F32D0;
extern RoomM086BattleEntity *D_8009D254;
extern RoomM086EventState *D_800E2368;
extern u8 *D_80190B84;
extern u16 D_800E11E8;
extern u16 D_800E11EA;
int func_800C6B90(void *position, int radius);
void func_800C6FA0(u8 *data, u16 factor);
u16 func_80077AA4(int, int);
int func_80077DC4(int angle);

#endif
