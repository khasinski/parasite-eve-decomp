#ifndef PE1_SCENE_GLOW_MODEL_H
#define PE1_SCENE_GLOW_MODEL_H

#include "pe1/room_fx.h"

/* Scene e08 glowing model: the effect model drawn at the scene anchor,
 * followed by a glow sprite that grows once the timer passes 10. */
typedef struct SceneGlowModelOwner {
    /* 0x00 */ u8 pad00[0x24];
    /* 0x24 */ int mode;
} SceneGlowModelOwner;

typedef struct SceneGlowModelTimer {
    /* 0x00 */ u8 pad00[2];
    /* 0x02 */ s16 ticks;
} SceneGlowModelTimer;

typedef struct SceneGlowModelState {
    /* 0x00 */ u8 pad00[2];
    /* 0x02 */ s16 height;
    /* 0x04 */ u8 pad04[4];
    /* 0x08 */ RoomFxSeed8 seed;
    /* 0x10 */ s16 scaleX;
    /* 0x12 */ s16 scaleY;
    /* 0x14 */ s16 scaleZ;
    /* 0x16 */ u8 pad16[2];
    /* 0x18 */ u16 depth;
} SceneGlowModelState;

typedef struct SceneGlowModelFrame {
    s16 value;
} SceneGlowModelFrame;

typedef struct SceneGlowModelColor {
    u8 bytes[8];
} SceneGlowModelColor;

extern u8 *D_80199528;
extern int D_8019956C;
extern int D_8019957C;
extern SceneGlowModelFrame D_800942EC;
extern u8 D_80198860[];
extern u8 D_801996B0[];

SceneGlowModelOwner *func_800C2B50(void);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_80071A44(RoomFxVec4 *vec, int value, int size);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C6D5C(u8 *packet, int x, int y);
int func_80077A64(int arg0, int arg1, int x, int y);
int func_80077AA4(int x, int y);
void func_800C6EC0(int tpage, int clut);
void func_800C3134(u8 *table, int index, SceneGlowModelColor *out);
void func_800C6ED8(int mode);
void func_800C6EF8(u8 *packet);
void func_800C6FA0(u8 *packet, int depth);
void func_800C71E4(u8 *packet, RoomSpriteMatrix *matrix);
void func_800C6F4C(u8 *packet);
void func_800C2EAC(int mode);
void func_800C2FF0(int width, int height);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(u8 *sprite, RoomSpriteMatrix *matrix, int mode);

#endif
