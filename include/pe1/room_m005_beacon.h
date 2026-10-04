#ifndef PE1_ROOM_M005_BEACON_H
#define PE1_ROOM_M005_BEACON_H

#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/field_model_draw.h"


/* room_m005 joint beacon (func_8018FDC4): follows joint 0x14 of the actor,
 * sheds drifting sprites, swells and shrinks, tags the player on contact and
 * draws four passes of a model, three glow sprites and a turning second
 * model while the effect starts. */
typedef struct RoomM005Beacon {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector rotation;
    /* 0x18 */ int height;
    /* 0x1C */ int width;
    /* 0x20 */ s16 phase;
    /* 0x22 */ s16 timer;
} RoomM005Beacon;

typedef struct RoomM005BeaconSprite {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 pad06;
    /* 0x08 */ s16 vx, vy, vz;
} RoomM005BeaconSprite;

typedef struct RoomM005BeaconSpawnChannel {
    s32 reserved[2];
    void *pool;
} RoomM005BeaconSpawnChannel;

typedef struct RoomM005BeaconNode {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM005BeaconNode;

typedef struct RoomM005BeaconPool {
    /* 0x000 */ RoomM005BeaconNode *node;
    /* 0x004 */ u8 reserved004[0x234];
    /* 0x238 */ GteMatrix *matrix;
} RoomM005BeaconPool;

typedef struct RoomM005BeaconChannel {
    s32 reserved[2];
    RoomM005BeaconPool *pool;
} RoomM005BeaconChannel;

typedef struct RoomM005BeaconActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM005BeaconActor;

typedef struct RoomM005BeaconEntity {
    RoomM005BeaconActor *actor;
} RoomM005BeaconEntity;

typedef struct RoomM005BeaconEvent {
    /* 0x00 */ u8 reserved[0xD];
    /* 0x0D */ u8 active;
    /* 0x0E */ u8 reserved0E[4];
    /* 0x12 */ s16 variant;
} RoomM005BeaconEvent;

typedef struct RoomM005BeaconOffset {
    s32 word[2];
} __attribute__((packed)) RoomM005BeaconOffset;

typedef struct RoomM005BeaconArchive {
    void *channel;
} RoomM005BeaconArchive;

/* The sprite parameter block at 0x800F3368 (RenderEffectParameters). */
typedef struct RoomM005BeaconParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomM005BeaconParams;

typedef struct RoomM005BeaconColor {
    u8 r, g, b, code;
} RoomM005BeaconColor;

typedef struct RoomM005BeaconMatrixSlot {
    s32 *value;
} RoomM005BeaconMatrixSlot;

extern RoomM005BeaconParams D_800F3368;
extern RoomM005BeaconMatrixSlot D_800BCFA4;
extern u16 D_800E1204[];
extern u16 D_800E2850[];
extern int D_800F3428;
extern int D_800E27EC;
extern RoomM005BeaconChannel *D_800F32D0;
extern RoomM005BeaconEvent *D_800E2368;
extern RoomM005BeaconEntity *D_8009D254;
extern RoomM005BeaconArchive D_800B0E64;
extern RoomM005BeaconOffset D_8018F010;
extern RoomM005BeaconSpawnChannel *D_800F33E0;
extern u8 *D_80190B94;
extern u8 *D_80190B98;
extern GteShortVector D_80190BA0;
extern GteShortVector D_80190BA8;
extern u16 D_800E11FA;
extern u16 D_800E11EA;
extern u16 D_800E11E4[];
extern s16 D_800942EC;

u8 *func_8006E498(void *channel, int id);
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
void func_800C6FA0(u8 *data, u16 factor);
void func_800C70EC(u8 *data, int r, int g, int b);
void func_800CFB7C(GteShortVector *angles, s16 distance, GteShortVector *out);
void func_800CEE20(GteShortVector *position, GteShortVector *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RoomM005BeaconColor *color);
void func_800CE9D4(void *owner, int index, void *out);
void func_800CE8F0(void *pool, int index, void *offset, void *position);
int func_800CEB8C(void *position, GteShortVector *target, int radius);
u16 GetClut(int x, int y);
int func_80077DC4(int angle);
int func_80077CF4(int angle);
int func_800CE560(void *pool, int count, int size, void *callback);
RoomM005BeaconSprite *func_800CE610(void *pool);
int func_80071A54(void);
int func_8018FB84(int mode, RoomM005BeaconSprite *sprite);

#endif
