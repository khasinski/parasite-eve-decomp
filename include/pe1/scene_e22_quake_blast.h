#ifndef PE1_SCENE_E22_QUAKE_BLAST_H
#define PE1_SCENE_E22_QUAKE_BLAST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_orbit_trail.h"
#include "pe1/gte.h"

/* Quake blast controller (scene_e22 func_80198348): a blast at the target
 * that lights the actor once it stands in reach, sprays sparks for six
 * frames, then draws a floor glow, a halo, a ring and three expanding
 * shock models. */
typedef struct SceneE22QuakeBlast {
    GteShortVector position;      /* 0x00 */
    u8 reserved08[8];
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
} SceneE22QuakeBlast;

typedef struct SceneE22QuakeTarget {
    s16 x, y, z, reserved06;      /* 0x00 */
    int radius;                   /* 0x08 */
} SceneE22QuakeTarget;

/* A colour written whole; the byte array keeps the union in memory
 * (BLKmode) so it gets its stack slot in declaration order. */
typedef struct SceneE22QuakeBytes {
    u8 rgb[3];
    u8 code;
} SceneE22QuakeBytes;

typedef union SceneE22QuakeColor {
    RenderColor color;
    SceneE22QuakeBytes bytes;
    u32 word;
} SceneE22QuakeColor;

typedef struct SceneE22QuakeMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} SceneE22QuakeMatrix;

typedef struct SceneE22QuakeScale {
    s32 x, y, z, reserved0C;
} SceneE22QuakeScale;

typedef struct SceneE22QuakeObject {
    u32 flags;                    /* 0x00 */
} SceneE22QuakeObject;

typedef struct SceneE22QuakeSlot {
    SceneE22QuakeObject *object;
} SceneE22QuakeSlot;

typedef struct SceneE22QuakeObjectChannel {
    s32 reserved[2];
    SceneE22QuakeSlot *pool;      /* 0x08 */
} SceneE22QuakeObjectChannel;

typedef struct SceneE22QuakeChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} SceneE22QuakeChannel;

typedef struct SceneE22QuakeEvent {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
} SceneE22QuakeEvent;

typedef struct SceneE22QuakeActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} SceneE22QuakeActor;

typedef struct SceneE22QuakeSound {
    void *channel;
} SceneE22QuakeSound;

typedef struct SceneE22QuakeFloor {
    u16 y;
} SceneE22QuakeFloor;

extern SceneE22QuakeSound D_800B0E64;
extern SceneE22QuakeEvent *D_800E2368;
extern SceneE22QuakeObjectChannel *D_800F32D0;
extern SceneE22QuakeChannel *D_800F33E0;
extern SceneE22QuakeActor **RoomMain_ActorPtr;
extern SceneE22QuakeFloor D_800942EC;
/* Texture page indices: [0] the shared effect page, [8] this scene's. */
extern u16 D_800E11EA[];
extern void *D_80199508;
extern void *D_8019950C;
extern u8 D_8019948C[];
extern GteRotation D_8018F1FC;
extern RenderColor D_8018F228;

extern int func_80071A54(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int, int);
extern s32 func_80077A64(s32, s32, s32, s32);
extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomOrbitTrailParticle *func_800CE610(void *pool);
extern int func_800D3FD8(void);
extern void func_8006DF50(void *channel, int id, int value, int volume, int pan);
extern void *func_8006E498(void *base, u32 key);
extern void func_800C6D5C(void *asset, int x, int y);
extern int func_800C6B90(void *position, int radius);
extern void func_800D1AE0(RenderColor *color, int value, int step, int count);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_80079754(void *rotation, void *matrix);
extern void func_80078CC4(void *matrix, void *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C6FA0(void *asset, int brightness);
extern void func_800C71E4(void *asset, void *matrix);
extern void func_800C6F4C(void *asset);
extern int func_801981B0(int mode, RoomOrbitTrailParticle *p);

#endif
