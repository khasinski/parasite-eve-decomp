#ifndef PE1_SCENE_E22_EMBER_BURST_H
#define PE1_SCENE_E22_EMBER_BURST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_orbit_trail.h"

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
} SceneE22EmberEvent;

typedef struct SceneE22EmberFloor {
    s16 y;
} SceneE22EmberFloor;

extern SceneE22EmberActor **RoomMain_ActorPtr;
extern SceneE22EmberEvent *D_800E2368;
extern SceneE22EmberObjectChannel *D_800F32D0;
extern SceneE22EmberChannel *D_800F33E0;
extern SceneE22EmberFloor D_800942EC;
extern u16 D_800E11EA;
extern u16 D_800E11FA;
extern RenderColor D_8018F208;
extern RenderColor D_8018F20C;

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

#endif
