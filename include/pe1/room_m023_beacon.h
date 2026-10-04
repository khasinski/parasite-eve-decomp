#ifndef PE1_ROOM_M023_BEACON_H
#define PE1_ROOM_M023_BEACON_H

#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/field_model_draw.h"


/* room_m023 joint beacon (func_8018FC14): follows a model joint, swells
 * and pulses through four phases, tags the player on contact and draws a
 * glow sprite pair, two star fans and a loaded model at the joint. */
typedef struct RoomM023Beacon {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector rotation;
    /* 0x18 */ int height;
    /* 0x1C */ int width;
    /* 0x20 */ int joint;
    /* 0x24 */ int armed;
    /* 0x28 */ s16 phase;
    /* 0x2A */ s16 timer;
    /* 0x2C */ s16 glow;
    /* 0x2E */ s16 shade;
} RoomM023Beacon;

typedef struct RoomM023BeaconNode {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM023BeaconNode;

typedef struct RoomM023BeaconPool {
    /* 0x000 */ RoomM023BeaconNode *node;
    /* 0x004 */ u8 reserved004[0x234];
    /* 0x238 */ GteMatrix *matrix;
} RoomM023BeaconPool;

typedef struct RoomM023BeaconChannel {
    s32 reserved[2];
    RoomM023BeaconPool *pool;
} RoomM023BeaconChannel;

typedef struct RoomM023BeaconActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM023BeaconActor;

typedef struct RoomM023BeaconEntity {
    RoomM023BeaconActor *actor;
} RoomM023BeaconEntity;

typedef struct RoomM023BeaconEvent {
    /* 0x00 */ u8 reserved[0xD];
    /* 0x0D */ u8 active;
    /* 0x0E */ u8 reserved0E[4];
    /* 0x12 */ s16 variant;
} RoomM023BeaconEvent;

typedef struct RoomM023BeaconOffset {
    s32 word[2];
} __attribute__((packed)) RoomM023BeaconOffset;

typedef struct RoomM023BeaconArchive {
    void *channel;
} RoomM023BeaconArchive;

/* The sprite parameter block at 0x800F3368 (RenderEffectParameters). */
typedef struct RoomM023BeaconParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomM023BeaconParams;

typedef struct RoomM023BeaconColor {
    u8 r, g, b, code;
} RoomM023BeaconColor;

typedef struct RoomM023BeaconMatrixSlot {
    s32 *value;
} RoomM023BeaconMatrixSlot;

extern RoomM023BeaconParams D_800F3368;
extern RoomM023BeaconMatrixSlot D_800BCFA4;
extern u16 D_800E1204[];
extern u16 D_800E2850[];
extern int D_800F3428;
extern int D_800E27EC;
extern RoomM023BeaconChannel *D_800F32D0;
extern RoomM023BeaconEvent *D_800E2368;
extern RoomM023BeaconEntity *D_8009D254;
extern RoomM023BeaconArchive D_800B0E64;
extern RoomM023BeaconOffset D_8018EFF4;
extern u8 *D_80190760;
extern u8 D_801906F0[];
extern u16 D_800E11EA;
extern s16 D_800942EC;

u8 *func_8006E498(void *channel, int id);
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
void func_800C6FA0(u8 *data, u16 factor);
void func_800C70EC(u8 *data, int r, int g, int b);
void func_800CFB7C(GteShortVector *angles, s16 distance, GteShortVector *out);
void func_800CEE20(GteShortVector *position, GteShortVector *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RoomM023BeaconColor *color);
void func_800D004C(GteShortVector *position, int width, int height, int segments,
                   GteShortVector *rotation, int scale_x, int scale_y,
                   RoomM023BeaconColor *color0, RoomM023BeaconColor *color1,
                   int intensity, int mode);
void func_800CF3AC(void *track, RoomM023BeaconColor *color, int time);
void func_800CE9D4(void *owner, int index, void *out);
void func_800CE8F0(void *pool, int index, void *offset, void *position);
int func_800CEB8C(void *position, GteShortVector *target, int radius);
u16 GetClut(int x, int y);
GteMatrix *MulMatrix0(GteMatrix *first, GteMatrix *second, GteMatrix *out);
int func_80077DC4(int angle);

#endif
