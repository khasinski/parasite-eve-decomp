#ifndef PE1_SCENE_PLAYER_ORB_H
#define PE1_SCENE_PLAYER_ORB_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/field_actor.h"

/* Scene e08 player orb: an effect spawned at a random spot around the
 * player, swung around the scene anchor by the anchor's facing, then drawn
 * as two colour-faded trails plus an eight-point sprite chain. */
typedef struct ScenePlayerOrbAnchor {
    /* 0x00 */ u8 pad00[0x18];
    /* 0x18 */ int x;
    /* 0x1C */ int y;
    /* 0x20 */ int z;
} ScenePlayerOrbAnchor;

typedef struct ScenePlayerOrbColor {
    u8 r, g, b, pad;
} ScenePlayerOrbColor;

typedef struct ScenePlayerOrb {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector start;
    /* 0x10 */ GteShortVector anchor;
    /* 0x18 */ u8 pad18[0x10];
    /* 0x28 */ GteShortVector points[8];
    /* 0x68 */ u8 trailA[0x18];
    /* 0x80 */ u8 trailB[0x18];
    /* 0x98 */ ScenePlayerOrbColor head;
    /* 0x9C */ ScenePlayerOrbColor tail;
    /* 0xA0 */ int alpha;
    /* 0xA4 */ int fade;
    /* 0xA8 */ u8 variant;
    /* 0xA9 */ u8 active;
    /* 0xAA */ u8 count;
} ScenePlayerOrb;

extern GteRotation D_8018EFFC;
extern FieldActor *g_PlayerEntity;
/* The sound owner read as a one-field record (see room_m089_spin_model.h). */
typedef struct ScenePlayerOrbSound {
    void *channel;
} ScenePlayerOrbSound;

extern ScenePlayerOrbSound D_800B0E64;

ScenePlayerOrbAnchor *func_800C2B50(void);
int *func_800C2B10(int index);
int func_80071A54(void);
void func_800794C4(GteRotation *rotation, GteMatrix *matrix);
void func_80078C34(GteMatrix *matrix, GteShortVector *in, GteShortVector *out);
void func_8006DF50(void *channel, int id, int value, int volume, int pan);

#endif
