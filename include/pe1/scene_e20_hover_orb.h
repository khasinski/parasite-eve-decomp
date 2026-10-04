#ifndef PE1_SCENE_E20_HOVER_ORB_H
#define PE1_SCENE_E20_HOVER_ORB_H

#include "common.h"

/* Scene e20 trail controller and its command/particle records.
 * Reserved members are observed offsets, not recovered field meanings. */
typedef struct SceneE20Vec { s16 x,y,z,pad; } SceneE20Vec;
typedef struct SceneE20Color { u8 r,g,b,code; } SceneE20Color;
typedef struct SceneE20Matrix { s16 m[3][3],pad; s32 t[3]; } SceneE20Matrix;
typedef struct SceneEffectSlot {
    SceneE20Vec position, target;
    int duration, pending;
} SceneEffectSlot;
typedef struct SceneE20Particle { SceneE20Vec position, velocity; s16 kind, timer; } SceneE20Particle;
typedef struct SceneE20TrailEffect {
    SceneE20Vec position, rotation, endpoint, target, previousPosition;
    SceneE20Vec commandedEndpoint, previousEndpoint, trailHead, trailTail;
    s16 phase, timer, alpha, reserved4E, duration, reserved52;
    struct { SceneE20Vec head, tail; } history[7];
} SceneE20TrailEffect;
typedef struct SceneE20Node { u32 flags; u8 reserved04[12]; s32 progress; } SceneE20Node;
typedef struct SceneE20Actor { u8 reserved[0x4c]; u32 flags; } SceneE20Actor;
typedef struct SceneE20Event { u8 reserved[13]; u8 active; } SceneE20Event;
typedef struct SceneE20Pool { SceneE20Node *node; u8 reserved04[0x264]; SceneE20Vec position; } SceneE20Pool;
typedef struct SceneE20Channel { int reserved[2]; SceneE20Pool *pool; } SceneE20Channel;
extern SceneE20Channel *D_800F32D0, *D_800F33E0;
extern SceneE20Color D_8018EFFC, D_8018F000, D_8018F004, D_8018F008;
extern short D_800942EC;
extern SceneE20Actor **D_8009D254;
extern void *D_800B0E64;
extern void *D_8019085C;
int func_8018F750(int mode, SceneE20TrailEffect *effect, SceneEffectSlot *command);
extern SceneEffectSlot D_80190860[];
int func_80071A54(void);
int func_800D3FD8(void);
void *func_8006E498(void *archive, unsigned key);
void func_8006DCE4(int sound, int volume, s16 x, s16 y, int z);
int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 func_80077A64(int tp, int abr, int x, int y);
u16 func_80077AA4(int x, int y);
void func_800783E4(SceneE20Vec *a, SceneE20Vec *b, int scaleA, int scaleB, SceneE20Vec *out);
void func_80078CC4(SceneE20Matrix *matrix, int *scale);
void func_80079754(SceneE20Vec *rotation, SceneE20Matrix *matrix);
int func_800C6B90(SceneE20Vec *position, int radius);
void func_800C6D5C(void *model, int a, int b);
SceneE20Particle *func_800CE610(SceneE20Pool *pool);
void func_800CE870(void *object, int joint, SceneE20Vec *out);
void func_800CFAA8(SceneE20Vec *from, SceneE20Vec *to, SceneE20Vec *angles);
void func_800CFB7C(SceneE20Vec *angles, s16 distance, SceneE20Vec *out);
void func_800D1384(SceneE20Vec *from, SceneE20Vec *to, u32 count, SceneE20Color *color0, SceneE20Color *color1, int scale, void *history, int mode);
void func_800D2B58(SceneE20Vec *from, SceneE20Vec *to, SceneE20Color *color0, SceneE20Color *color1, int scale0, int scale1, int mode);

void func_800C6EC0(int tpage,int clut);
void func_800C6ED8(int mode);
void func_800C6EF8(void *model);
void func_800C6F4C(void *model);
void func_800C6FA0(void *model, u16 alpha);
void func_800C71E4(void *model, SceneE20Matrix *matrix);
int func_8018F028(int, SceneE20Particle *);
int func_800CE560(SceneE20Pool *pool,int stride,int count,void *callback);
void func_800CEE20(SceneE20Vec *position,SceneE20Vec *rotation,int scaleX,int scaleY,int cell,int clut,int mode,int alpha,SceneE20Color *color);
extern SceneE20Event *D_800E2368;
extern u16 D_800E1204[],D_800E2850[];
extern SceneE20Matrix *D_800BCFA4;

extern u16 firstPageTable[] asm("D_800E2850"); /* Same linker symbol, distinct compiler name. */
extern u16 D_800E11EA;
extern u16 D_800E11FA;
extern u16 D_800E120A;
extern s32 D_800E27EC;
extern s16 D_800F3368;
extern s16 D_800F336A;
extern u16 D_800F336C;
extern s16 D_800F336E;
extern u16 D_800F3370;
extern s16 D_800F3372;
extern s16 D_800F3374;
extern s16 D_800F3376;
extern s16 D_800F3378;
extern s32 D_800F3428;
extern s32 D_80190800;

PE1_STATIC_ASSERT(sizeof(SceneEffectSlot) == 0x18, scene_e20_slot_size);
PE1_STATIC_ASSERT(sizeof(SceneE20Particle) == 0x14, scene_e20_particle_size);
PE1_STATIC_ASSERT(sizeof(SceneE20TrailEffect) == 0xC4, scene_e20_trail_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE20TrailEffect, history) == 0x54, scene_e20_history_offset);

#endif
