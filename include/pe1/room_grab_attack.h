#ifndef PE1_ROOM_GRAB_ATTACK_H
#define PE1_ROOM_GRAB_ATTACK_H

#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/room_motion_trigger.h"
#include "pe1/room_spark.h"
#include "pe1/room_sound_slot.h"
#include "pe1/room_ground_sweep.h"
#include "pe1/room_orbit_trail.h"
#include "pe1/room_rising_spray.h"

/* The grab attack set (src/overlays/room_lib/RoomFx_GrabAttackSet.c, in
 * room_m269 and scene_e01). */

/* The class's state words from 0x0C on, as the player reset receives them. */
typedef struct RoomGrabStateTail {
    /* 0x00 */ void (*callback)();
    /* 0x04 */ s32 *completion_state;
    /* 0x08 */ u8 reserved08[8];
    /* 0x10 */ FieldActor *source_actor;
    /* 0x14 */ s32 saved_x;
    /* 0x18 */ s32 saved_z;
    /* 0x1C */ u8 activated;
} RoomGrabStateTail;

/* A 16.16 world point. */
typedef struct RoomGrabPoint {
    s32 x, y, z;
} RoomGrabPoint;

/* The spray controller's parameters: base launch speed and heading spread. */
typedef struct RoomRisingSprayParams {
    s32 speed;
    s32 spread;
} RoomRisingSprayParams;

/* Per-overlay data, named in each symbol file: the side the sweep starts
 * on, the orbit centre the trails sweep out from, the trails' track, the
 * slot and the pair of attack arguments the setters fill. */
extern s32 g_RoomGrabSweepSide;
extern GteShortVector g_RoomGrabOrbitCentre;
extern u8 g_RoomGrabTrailTrack[];
extern int g_RoomGrabSlot;
extern int RoomLib_PairA;
extern int RoomLib_PairB;

/* The player pointer, from room_spark.h, read as volatile in this unit:
 * the hold reloads it after every store through it, as retail does, and
 * the other functions copy it to a local where retail loads it once. GCC
 * merges the added qualifier into room_spark.h's declaration; no other
 * unit sees it. */
extern RoomSparkBattleEntity *volatile D_8009D254;

extern FieldActor *D_8009D20C;
/* Read and written as a volatile word by the release. */
extern volatile int D_800BCF88;

int RoomFx_GrabNop0(void);
int RoomFx_GrabInit(RoomMotionTrigger *obj);
int RoomFx_GrabConfigure(char *obj, int arg1, int op, int arg3, int arg4);
int RoomFx_GrabNop3(void);
int RoomFx_GrabUpdate(RoomMotionTrigger *obj);
void RoomFx_GrabAwaitTarget(RoomMotionTrigger *obj);
void RoomFx_GrabStartOnProximity(RoomMotionTrigger *arg);
void RoomFx_GrabHold(RoomMotionTrigger *arg0);
void RoomFx_GrabResetPlayer(FieldActor *unused, RoomGrabStateTail *state);
int RoomFx_GrabRelease(RoomMotionTrigger *entity);
int RoomFx_GrabNop6(void);
int RoomFx_GroundSweepMark(int mode, RoomGroundSweepMark *state);
int RoomFx_OrbitTrailParticle(int mode, RoomOrbitTrailParticle *p);
int RoomFx_RisingSprayParticle(int mode, RoomSpraySpark *spark);

void func_8003E0FC(void *object, int count, void *out);
s32 func_800DFE20(RoomGrabPoint *lhs, RoomGrabPoint *rhs);
void func_80020C74(void);
void func_80020CE4(void);
s32 func_8003010C(void *actor, s32 arg1);
void func_8001AA78(FieldActor *actor);
extern int func_8006DF50_handle(void *channel, int id, int value, int volume,
                                int pan) __asm__("func_8006DF50");
void func_800866A4(int handle, int arg);
void func_800D2B58(void *, void *, void *, void *, int, int, int);
void func_800D1384(void *from, void *to, int width, void *color0,
                   void *color1, int alpha, void *trail, int mode);

#endif
