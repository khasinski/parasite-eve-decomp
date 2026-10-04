#ifndef PE1_SCENE_RING_SPRITES_H
#define PE1_SCENE_RING_SPRITES_H

#include "common.h"
#include "pe1/gte_types.h"

/* Scene e08 ring sprites: an animated flash sprite (first 12 ticks), a
 * steady glow sprite, and two shaded rings scaled by the state's size. */
typedef struct SceneRingSpritesOwner {
    /* 0x00 */ u8 pad00[0x24];
    /* 0x24 */ u8 mode;
} SceneRingSpritesOwner;

typedef struct SceneRingSpritesState {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 z;
    /* 0x06 */ u8 pad06[2];
    /* 0x08 */ s16 size;
    /* 0x0A */ s16 depth;
    /* 0x0C */ u8 ticks;
} SceneRingSpritesState;

/* Sprite parameter block drawn by func_800C42A4. */
typedef struct SceneRingSprite {
    /* 0x00 */ u8 r, g, b, pad3;
    /* 0x04 */ u8 texture, mode, extra, pad7;
    /* 0x08 */ s16 offset;
    /* 0x0A */ s16 depth;
    /* 0x0C */ u8 pad0C[4];
} SceneRingSprite;

/* Shaded ring parameter block drawn by func_800C4FC4. */
typedef struct SceneRingShade {
    /* 0x00 */ u8 pad00[0x14];
    /* 0x14 */ s16 depth;
    /* 0x16 */ u8 pad16[2];
} SceneRingShade;

extern GteRotation D_8018F018;
extern GteVector D_8018F020;
extern GteVector D_8018F030;
extern SceneRingSprite D_801995A8;
extern SceneRingSprite D_801995B8;
extern SceneRingShade D_801995C8[2];

SceneRingSpritesOwner *func_800C2B50(void);
void func_800C2EAC(int mode);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C2FF0(int width, int height);
void func_80078CC4(GteMatrix *matrix, GteVector *scale);
void func_800C42A4(SceneRingSprite *sprite, GteMatrix *matrix, int mode);
void func_800794C4(GteRotation *rotation, GteMatrix *matrix);
void func_80071A44(GteVector *vector, int value, int size);
void func_800C4FC4(SceneRingShade *ring, GteMatrix *matrix, int mode);

#endif
