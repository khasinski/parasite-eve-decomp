#ifndef PE1_SCENE_PLAYER_ORB_H
#define PE1_SCENE_PLAYER_ORB_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/field_actor.h"
#include "pe1/render_object.h"

/* Scene e08 player orb: an effect spawned at a random spot around the
 * player, swung around the scene anchor by the anchor's facing, then drawn
 * as two colour-faded trails plus an eight-point sprite chain. */
typedef struct ScenePlayerOrbAnchor {
    /* 0x00 */ u8 pad00[0x18];
    /* 0x18 */ int x;
    /* 0x1C */ int y;
    /* 0x20 */ int z;
    /* 0x24 */ u8 mode;
    /* 0x25 */ u8 pad25[7];
    /* 0x2C */ s16 orbHit;
} ScenePlayerOrbAnchor;

typedef struct ScenePlayerOrbTimer {
    /* 0x00 */ u8 pad00;
    /* 0x01 */ u8 state;
} ScenePlayerOrbTimer;

/* Frame counter read as a one-field record so each read stays below the
 * preceding stores into the orb. */
typedef struct ScenePlayerOrbFrame {
    s16 value;
} ScenePlayerOrbFrame;

typedef struct ScenePlayerOrb {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector start;
    /* 0x10 */ GteShortVector anchor;
    /* 0x18 */ u8 pad18[0x10];
    /* 0x28 */ GteShortVector points[8];
    /* 0x68 */ GteShortVector trailA[3];
    /* 0x80 */ GteShortVector trailB[3];
    /* 0x98 */ RenderColor head;
    /* 0x9C */ RenderColor tail;
    /* 0xA0 */ int alpha;
    /* 0xA4 */ int fade;
    /* 0xA8 */ u8 delay;
    /* 0xA9 */ u8 active;
    /* 0xAA */ u8 count;
} ScenePlayerOrb;

extern GteRotation D_8018EFFC;
extern RenderColor D_8018F004;
extern GteVector D_8018F008;
extern u8 D_801987E4[];
extern u8 D_80198718[];
extern u8 D_80198754[];
extern ScenePlayerOrbFrame D_800942EC;
extern u8 D_80199690[];
extern FieldActor *g_PlayerEntity;
/* The sound owner read as a one-field record (see room_m089_spin_model.h). */
typedef struct ScenePlayerOrbSound {
    void *channel;
} ScenePlayerOrbSound;

extern ScenePlayerOrbSound D_800B0E64;

ScenePlayerOrbAnchor *func_800C2B50(void);
int *func_800C2B10(int index);
GteShortVector *func_800C2B90(void *object, int kind, u8 *script, u8 *data);
int func_800C6B90(GteShortVector *position, int radius);
int func_80071A54(void);
void func_800794C4(GteRotation *rotation, GteMatrix *matrix);
void func_80078C34(GteMatrix *matrix, GteShortVector *in, GteShortVector *out);
void func_800C2EAC(int mode);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C2FF0(int width, int height);
void func_800C3134(u8 *table, int index, RenderColor *out);
int func_80077A64(int arg0, int arg1, int x, int y);
int func_80077AA4(int x, int y);
void func_800D3114(GteShortVector *trail, int last, int arg2, int arg3, int r, int g, int b,
                   int tpage, int clut, int alpha, RenderColor *head,
                   RenderColor *tail, int mode);
void func_80078CC4(GteMatrix *matrix, GteVector *scale);
void func_800C42A4(u8 *sprite, GteMatrix *matrix, int mode);
void func_8006DF50(void *channel, int id, int value, int volume, int pan);

/* Scene e08 limb beams: ten ribbons fanned from the player's chest bone to
 * hand and foot bones, each with its own swing and bend offsets. */
typedef struct SceneLimbBeamShape {
    u16 rise;
    u16 bend;
    u16 reach;
    u16 swing;
} SceneLimbBeamShape;

typedef struct SceneLimbBeams {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s16 fade[10];
    /* 0x20 */ u8 pad20[4];
    /* 0x24 */ u8 mode[10];
} SceneLimbBeams;

extern SceneLimbBeamShape D_801994D8[10];
extern u16 D_80199658[10];
extern u8 D_801989BC[];
extern u8 D_801989D0[];
int func_80077CF4(int angle);
int func_80077DC4(int angle);

#endif
