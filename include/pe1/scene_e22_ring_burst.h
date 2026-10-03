#ifndef PE1_SCENE_E22_RING_BURST_H
#define PE1_SCENE_E22_RING_BURST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_orbit_trail.h"

/* Ring burst controller (scene_e22): scene event gate, particle channel
 * and the helpers the burst and its orbit trail particles call. */

typedef struct SceneE22BurstEvent {
    u8 reserved[0x16];
    s16 running;                  /* 0x16 */
} SceneE22BurstEvent;

typedef struct SceneE22BurstChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} SceneE22BurstChannel;

extern SceneE22BurstEvent *D_800E2368;
extern SceneE22BurstChannel *D_800F33E0;
extern char RoomMain_ActorPtr[];
extern u16 D_800E11EA;
extern u16 D_800E11FA;
extern RenderColor D_8018F1D0;
extern RenderColor D_8018F1D4;
extern GteShortVector D_801994EC;
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int, int);
extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomOrbitTrailParticle *func_800CE610(void *pool);
extern int func_80071A54(void);
extern int func_800D3FD8(void);
extern int func_800D3F64(int sound, int handle);
extern int func_80192548(int mode, RoomOrbitTrailParticle *p);

/* Orbit ring trail particle (func_80192548): the floor it bounces on, its
 * colour seed and trail track, and the trail strip renderer. */
typedef struct SceneE22FrameCounter {
    s16 count;
} SceneE22FrameCounter;

extern SceneE22FrameCounter D_800942EC;
extern RenderColor D_8018F1CC;
extern u8 D_80199168[];
extern void func_800D2B58(void *, void *, void *, void *, int, int, int);

/* Damped glow particle (func_801931B8): its glow colour track. */
extern u8 D_80199190[];

/* Twisting trail particle (func_801962FC): spin template, colour track and
 * the bent trail renderer. */
extern GteRotation D_8018F1F4;
extern u8 D_8019944C[];
extern void func_800CF844(void *, void *, int, void *, int, int);

#endif
