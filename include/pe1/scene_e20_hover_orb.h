#ifndef PE1_SCENE_E20_HOVER_ORB_H
#define PE1_SCENE_E20_HOVER_ORB_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/gte.h"

/* scene_e20 hover orb controller (func_8018F750): a spinning model orb with
 * a beam between two flares. It picks a random spot around the actor, glides
 * there, waits for a target, flies at it, bursts into a spark fan, then
 * damages the actor if it is in reach. */
typedef struct SceneE20HoverOrb {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector rotation;
    /* 0x10 */ GteShortVector target;
    /* 0x18 */ GteShortVector destination;
    /* 0x20 */ GteShortVector origin;
    /* 0x28 */ GteShortVector targetDestination;
    /* 0x30 */ GteShortVector targetOrigin;
    /* 0x38 */ GteShortVector flareA;
    /* 0x40 */ GteShortVector flareB;
    /* 0x48 */ s16 state;
    /* 0x4A */ s16 timer;
    /* 0x4C */ s16 intensity;
    /* 0x4E */ s16 reserved4E;
    /* 0x50 */ s16 duration;
    /* 0x52 */ s16 reserved52;
    /* 0x54 */ u8 beam[4];
} SceneE20HoverOrb;

/* Target handed over by the scene script. */
typedef struct SceneE20HoverTarget {
    /* 0x00 */ GteShortVector destination;
    /* 0x08 */ GteShortVector rotation;
    /* 0x10 */ int duration;
    /* 0x14 */ int pending;
} SceneE20HoverTarget;

/* Spark spawned by the orb (func_8018F028). */
typedef struct SceneE20HoverSpark {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector heading;
    /* 0x10 */ s16 state;
    /* 0x12 */ s16 timer;
} SceneE20HoverSpark;

typedef struct SceneE20HoverColor {
    u8 r, g, b, code;
} SceneE20HoverColor;

typedef struct SceneE20HoverObject {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0xC];
    /* 0x10 */ int health;
    /* 0x14 */ u8 reserved14[8];
    /* 0x1C */ u8 actions[0x80];
} SceneE20HoverObject;

typedef struct SceneE20HoverPool {
    /* 0x000 */ SceneE20HoverObject *object;
    /* 0x004 */ u8 reserved004[0x264];
    /* 0x268 */ GteShortVector position;
} SceneE20HoverPool;

typedef struct SceneE20HoverChannel {
    s32 reserved[2];
    SceneE20HoverPool *pool; /* 0x08 */
} SceneE20HoverChannel;

typedef struct SceneE20HoverActor {
    u8 reserved[0x4C];
    u32 flags; /* 0x4C */
} SceneE20HoverActor;

typedef struct SceneE20HoverBattle {
    SceneE20HoverActor *actor;
} SceneE20HoverBattle;

typedef struct SceneE20HoverEvent {
    u8 reserved[0xD];
    u8 active; /* 0x0D */
} SceneE20HoverEvent;

typedef struct SceneE20HoverFloor {
    u16 count;
} SceneE20HoverFloor;

typedef struct SceneE20HoverMatrixSlot {
    s32 *value;
} SceneE20HoverMatrixSlot;

/* The sprite parameter block at 0x800F3368. */
typedef struct SceneE20HoverParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} SceneE20HoverParams;

extern SceneE20HoverParams D_800F3368;
extern SceneE20HoverMatrixSlot D_800BCFA4;
extern SceneE20HoverChannel *D_800F32D0;
extern SceneE20HoverChannel *D_800F33E0;
extern SceneE20HoverBattle *D_8009D254;
extern SceneE20HoverEvent *D_800E2368;
extern SceneE20HoverFloor D_800942EC;
/* Model archive base, read as a one-field record so the load stays behind
 * the in-struct stores. */
typedef struct SceneE20HoverArchive {
    void *base;
} SceneE20HoverArchive;

extern SceneE20HoverArchive D_800B0E64;
/* Palette of the parameter block, read back through its own symbol. */
extern u16 D_800F336C;
extern int D_800E27EC;
extern int D_800F3428;
extern u16 D_800E11EA;
extern u16 D_800E11FA;
extern u16 D_800E120A;
extern u16 D_800E1204[];
extern u16 D_800E2850[];
extern SceneE20HoverColor D_8018EFFC;
extern SceneE20HoverColor D_8018F000;
extern SceneE20HoverColor D_8018F004;
extern SceneE20HoverColor D_8018F008;
extern u8 *D_8019085C;
extern int D_80190800;

int func_8018F028(int mode, SceneE20HoverSpark *spark);
int func_80071A54(void);
int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 func_80077AA4(int x, int y);
u16 GetTPage(int tp, int abr, int x, int y);
void GsSetOrign(int tpage, int clut);
void *func_8006E498(void *base, u32 key);
void func_8006DCE4(int id, int channel, int x, int y, int z);
int func_800D3FD8(void);
void func_800C6D5C(u8 *model, int x, int y);
void func_800C6ED8(int mode);
void func_800C6EF8(u8 *model);
void func_800C6F4C(u8 *model);
void func_800C6FA0(u8 *model, u16 factor);
void func_800C71E4(u8 *model, GteMatrix *matrix);
int func_800C6B90(GteShortVector *position, int radius);
int func_800CE560(SceneE20HoverPool *pool, int size, int count, void *callback);
SceneE20HoverSpark *func_800CE610(SceneE20HoverPool *pool);
void func_800CE870(SceneE20HoverBattle *object, int mode, GteShortVector *position);
void func_800CFAA8(GteShortVector *from, GteShortVector *to, GteShortVector *angles);
void func_800CFB7C(GteShortVector *angles, int distance, GteShortVector *out);
void func_800CEE20(GteShortVector *position, GteShortVector *rotation, int scale_x,
                   int scale_y, int texture, int clut, int page, int intensity,
                   SceneE20HoverColor *color);
void func_800D1384(GteShortVector *from, GteShortVector *to, int width,
                   SceneE20HoverColor *color0, SceneE20HoverColor *color1,
                   int intensity, u8 *beam, int mode);
void func_800D2B58(GteShortVector *from, GteShortVector *to, SceneE20HoverColor *color0,
                   SceneE20HoverColor *color1, int width0, int width1, int mode);

#endif
