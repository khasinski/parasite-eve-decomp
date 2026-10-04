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
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM086Object;

typedef struct RoomM086Pool {
    /* 0x000 */ RoomM086Object *object;
    /* 0x004 */ u8 reserved004[0x234];
    /* 0x238 */ void *bone;
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

extern int D_800E27EC;

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
void func_800CEE20(GteShortVector *position, GteShortVector *rotation,
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

/* room_m086 seeker controller (func_8018F734): follows joint 7 of the
 * actor, drifts and falls, then bursts into three seeker rings. */
typedef struct RoomM086SeekerController {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector velocity;
    /* 0x10 */ GteShortVector rotation;
    /* 0x18 */ s16 state;
    /* 0x1A */ s16 timer;
} RoomM086SeekerController;

typedef struct RoomM086ControllerParams {
    /* 0x00 */ u32 size;
    /* 0x04 */ s32 reserved04;
    /* 0x08 */ s32 spacing;
    /* 0x0C */ u32 count;
} RoomM086ControllerParams;

typedef struct RoomM086Offset {
    s16 x, y, z, pad;
} RoomM086Offset;

typedef struct RoomM086SpawnChannel {
    s32 reserved[2];
    void *pool;
} RoomM086SpawnChannel;

typedef struct RoomM086MatrixSlot {
    s32 *value;
} RoomM086MatrixSlot;

extern RoomM086MatrixSlot D_800BCFA4;
extern RoomM086SpawnChannel *D_800F33E0;
extern RoomM086Offset D_8018EFF4;
extern RoomM086Offset D_8018EFFC;
/* Model archive base, read twice through one address register. */
typedef struct RoomM086Archive {
    void *base;
} RoomM086Archive;

extern RoomM086Archive D_800B0E64;
extern u8 *D_80190B88;
extern u16 D_800E1208;
extern u16 D_800E120A;

int func_8018F004(int mode, RoomM086Seeker *seeker, RoomM086SeekerParams *params);
void func_800CE8F0(void *owner, int joint, RoomM086Offset *offset, void *out);
void func_800CEAE8(void *bone, RoomM086Offset *offset, void *out);
void func_800CE9D4(void *owner, int index, void *out);
RoomM086Seeker *func_800CE610(void *pool);
int func_800CE560(void *pool, int count, int size, void *callback);
void *func_8006E498(void *base, u32 key);
void func_800C6D5C(void *asset, int x, int y);
int func_800D3FD8(void);
void func_800D3F64(int sound, int handle);
int func_80071A54(void);
int func_80077CF4(int angle);

#endif
