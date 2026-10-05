#ifndef PE1_SCENE_E19_BLAST_H
#define PE1_SCENE_E19_BLAST_H

#include "pe1/render_object.h"
#include "pe1/gte.h"

/* Scene e19 blast sequence: loads five shell models, then runs a five-state
 * charge/burst timeline at an actor position, spawning rising sparks while
 * the shells, glow rings and flares expand and fade. */
typedef struct SceneE19Blast {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector origin;
    /* 0x18 */ s16 state;
    /* 0x1A */ s16 timer;
} SceneE19Blast;

typedef struct SceneE19BlastSpark {
    /* 0x00 */ u16 x, y, z;
    /* 0x06 */ s16 pad;
    /* 0x08 */ s16 state;
    /* 0x0A */ s16 timer;
} SceneE19BlastSpark;

typedef struct SceneE19BlastActor {
    u32 flags;
} SceneE19BlastActor;

typedef struct SceneE19BlastChannel {
    s32 reserved[2];
    SceneE19BlastActor **pool; /* 0x08 */
} SceneE19BlastChannel;

typedef struct SceneE19BlastPlayer {
    u8 reserved[0x4C];
    s32 flags; /* 0x4C */
} SceneE19BlastPlayer;

typedef struct SceneE19BlastScript {
    u8 reserved[0xD];
    u8 active; /* 0x0D */
} SceneE19BlastScript;

typedef struct SceneE19BlastSound {
    void *channel;
} SceneE19BlastSound;

extern GteRotation D_8018F1D4;
extern RenderColor D_8018F1DC, D_8018F1E0;
extern SceneE19BlastSound D_800B0E64;
extern SceneE19BlastChannel *D_800F32D0, *D_800F33E0;
extern SceneE19BlastPlayer **D_8009D254;
extern SceneE19BlastScript *D_800E2368;
extern u16 D_800942EC;
extern u16 D_800E11EA[];
extern u16 D_800E120A, D_800E11FA;
extern s16 D_800F336A, D_800F336E, D_800F3372, D_800F3374;
extern u16 D_800F3370, D_800F3376, D_800F3378;
extern s32 D_8019B668;
extern void *D_8019B680, *D_8019B684, *D_8019B688, *D_8019B68C, *D_8019B690;

int func_80192F9C(int mode, SceneE19Blast *blast);
int func_80192E08(int mode, void *spark);
void *func_8006E498(void *channel, u32 key);
void func_8006DF50(void *channel, int sound, int time, int pan, int volume);
int func_80071A54(void);
int func_80077A64(int tp, int abr, int x, int y);
u16 func_80077AA4(int x, int y);
int func_80077CF4(int angle);
int func_80077DC4(int angle);
void func_80078CC4(GteMatrix *matrix, GteVector *scale);
void func_80079754(GteShortVector *angles, GteMatrix *matrix);
int func_800C6B90(GteShortVector *position, int range);
void func_800C6D5C(void *model, int x, int y);
void func_800C6EC0(int page, int clut);
void func_800C6ED8(int mode);
void func_800C6EF8(void *model);
void func_800C6F4C(void *model);
void func_800C6FA0(void *model, int intensity);
void func_800C71E4(void *model, GteMatrix *matrix);
int func_800CE560(void *pool, int size, int count, void *callback);
SceneE19BlastSpark *func_800CE610(void *pool);
void func_800D1AE0(RenderColor *color, int value, int mode, int step);
void func_800D1D24(int mode, int count, int value);
int func_800D3FD8(void);

#endif
