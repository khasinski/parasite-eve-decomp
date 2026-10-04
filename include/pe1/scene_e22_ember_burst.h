#ifndef PE1_SCENE_E22_EMBER_BURST_H
#define PE1_SCENE_E22_EMBER_BURST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_orbit_trail.h"
#include "pe1/scene_e22_floor.h"

/* Ember controllers (scene_e22): ride the room actor, wake the scene
 * object's status byte, spray ember drift particles and draw a halo and a
 * ring while they glow. */

typedef struct SceneE22EmberBurst {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 state;                    /* 0x08 */
    s16 timer;                    /* 0x0A */
    s16 glow;                     /* 0x0C */
} SceneE22EmberBurst;

typedef struct SceneE22EmberActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} SceneE22EmberActor;

typedef struct SceneE22EmberObject {
    u32 flags;                    /* 0x00 */
    u8 reserved04[0x14];
    u8 *status;                   /* 0x18 */
} SceneE22EmberObject;

typedef struct SceneE22EmberSlot {
    SceneE22EmberObject *object;
} SceneE22EmberSlot;

typedef struct SceneE22EmberObjectChannel {
    s32 reserved[2];
    SceneE22EmberSlot *pool;      /* 0x08 */
} SceneE22EmberObjectChannel;

typedef struct SceneE22EmberChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} SceneE22EmberChannel;

typedef struct SceneE22EmberEvent {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
    u8 reserved0E[4];
    s16 jointSet;                 /* 0x12 */
} SceneE22EmberEvent;

typedef SceneE22FloorHeight SceneE22EmberFloor;

extern SceneE22EmberActor **RoomMain_ActorPtr;
extern SceneE22EmberEvent *D_800E2368;
extern SceneE22EmberObjectChannel *D_800F32D0;
extern SceneE22EmberChannel *D_800F33E0;
extern SceneE22EmberFloor D_800942EC;
/* Texture page index slot at 0x800E11EA, read as a record so the load
 * stays behind the first parameter block store through its base register. */
typedef struct SceneE22TextureSlot {
    u16 index;
} SceneE22TextureSlot;

extern SceneE22TextureSlot D_800E11EA;
/* The scene's own page index at 0x800E11FA, read the same way. */
extern SceneE22TextureSlot D_800E11FA;
extern RenderColor D_8018F208;
extern RenderColor D_8018F20C;
extern RenderColor D_8018F204;
extern u16 D_800E120A;
extern u8 D_80199388[];

extern int func_80071A54(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int, int);
extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomOrbitTrailParticle *func_800CE610(void *pool);
extern int func_800D3FD8(void);
extern int func_800D3F64(int sound, int handle);
extern void func_80020D50(void);
extern void func_80020DD0(void);
extern int func_80193940(int mode, RoomOrbitTrailParticle *p);

/* Link beam controller (func_80193414): two light ribbons strung across
 * three actor joints picked by the scene event's joint set; the last joint
 * sheds glow sparks along its heading, then the ribbons fade. */
typedef struct SceneE22LinkBeam {
    s16 reserved00;               /* 0x00 */
    s16 timer;                    /* 0x02 */
    s16 alpha;                    /* 0x04 */
    s16 jointSet;                 /* 0x06 */
    u8 trailA[0xB0];              /* 0x08 */
    u8 trailB[0xB0];              /* 0xB8 */
} SceneE22LinkBeam;

typedef struct SceneE22LinkJoints {
    s16 first, middle, last;
} SceneE22LinkJoints;

extern SceneE22LinkJoints D_801991B8[];
extern GteShortVector D_8018F1D8;
extern GteShortVector D_8018F1E0;
extern RenderColor D_8018F1E8;
extern RenderColor D_8018F1EC;
extern RenderColor D_8018F1F0;
extern void func_800CE8F0(void *pool, int index, void *offset, void *position);
extern void func_800D1384(void *from, void *to, int width, void *color0,
                          void *color1, int alpha, void *trail, int mode);
extern int func_801931B8(int mode, RoomOrbitTrailParticle *p);

/* Swirl ring controller (func_80195B40): rides an actor joint, swells a
 * glow while it sprays swirl ring particles, then fades a halo and ring. */
typedef struct SceneE22SwirlRing {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 state;                    /* 0x08 */
    s16 timer;                    /* 0x0A */
    s16 glow;                     /* 0x0C */
    s16 ring;                     /* 0x0E */
} SceneE22SwirlRing;

/* The sound owner read as a one-field record so the twist model's read
 * stays behind its state clears (see room_m089_spin_model.h). */
typedef struct SceneE22SoundOwner {
    void *channel;
} SceneE22SoundOwner;

extern SceneE22SoundOwner D_800B0E64;
extern GteRotation D_8018F1F4;
extern RenderColor D_8018F214;
extern u8 D_80199434[];
extern void func_8006DF50(void *channel, int id, int value, int volume, int pan);
extern void func_800D1AE0(RenderColor *color, int value, int step, int count);
extern int func_801957CC(int mode, RoomOrbitTrailParticle *p);

/* Ember column controller (func_80194F60): its spin template and halo
 * colour, and the rising ember particle it sprays. */
extern GteRotation D_8018F1FC;
extern u8 D_80199308[];
extern u8 D_80199190[];
extern RenderColor D_8018F210;
extern int func_8019485C(int mode, RoomOrbitTrailParticle *p);

/* Twist model controller (func_80196554): a model bolt that rides an
 * actor joint along its heading, sheds twisting trail particles, then
 * draws as a spinning model with glows and flares. */
typedef struct SceneE22TwistModel {
    GteShortVector heading;       /* 0x00 */
    GteShortVector position;      /* 0x08 */
    GteShortVector origin;        /* 0x10 */
    s16 state;                    /* 0x18 */
    s16 timer;                    /* 0x1A */
    s16 reserved1C;               /* 0x1C */
} SceneE22TwistModel;

typedef struct SceneE22ModelMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} SceneE22ModelMatrix;

typedef struct SceneE22ModelScale {
    s32 x, y, z, reserved0C;
} SceneE22ModelScale;

extern void *D_80199500;
extern RenderColor D_8018F218;
extern void *func_8006E498(void *base, u32 key);
extern void func_800C6D5C(void *asset, int x, int y);
extern s32 func_80077A64(s32, s32, s32, s32);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_80079754(void *rotation, void *matrix);
extern void func_80078CC4(void *matrix, void *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C6FA0(void *asset, int brightness);
extern void func_800C71E4(void *asset, void *matrix);
extern void func_800C6F4C(void *asset);
extern void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y,
                          int texture, int clut, int page, int intensity, int arg7,
                          int size);
extern int func_801962FC(int mode, RoomOrbitTrailParticle *p);

/* Sweep bolt controller (func_8019753C): the twist model's sibling that
 * sweeps the bolt out from its origin, sheds twist beam particles, then
 * hovers with a ring and halo. */
typedef struct SceneE22SweepBolt {
    GteShortVector heading;       /* 0x00 */
    GteShortVector origin;        /* 0x08 */
    GteShortVector position;      /* 0x10 */
    s16 state;                    /* 0x18 */
    s16 timer;                    /* 0x1A */
    s16 reserved1C;               /* 0x1C */
    s16 sweep;                    /* 0x1E */
} SceneE22SweepBolt;

extern RenderColor D_8018F224;
extern s16 D_80199504;
extern void func_800D1D24(int mode, int step, int time);
extern int func_8019702C(int mode, RoomOrbitTrailParticle *p);

#endif
