#ifndef PE1_ROOM_ORBIT_SET_H
#define PE1_ROOM_ORBIT_SET_H

#include "common.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"

/* The orbit effect set linked by room_m186, m187, m385, m388 and m389
 * (src/overlays/room_lib/RoomFx_OrbitEffectSet.c): eight orbiting
 * particles, a burst of eight falling sprites with floor shadows and one
 * drifting sprite, sharing four sprite records the set's init fills. */

/* Per-room data: the four sprite records, at different addresses and with
 * different gaps in each room. */
extern RoomFxSpritePacket g_RoomOrbitSpritePacket;
extern RoomFxSpritePacket g_RoomOrbitParticlePacket;
extern RoomFxSpritePacket g_RoomOrbitBurstPacket;
extern RoomFxSpritePacket g_RoomOrbitShadowPacket;

/* The frame counter at 0x800942EC, read as the floor height. */
extern short D_800942EC;

void *func_8006DC18(int size);
void func_800C2B40(void *state);
int *func_800C2B10(int index);
int func_800C2B68(void);
int func_800C66C8(void *object, int message, void *state);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);
void func_80071A44(void *dst, int value, int size);
int func_80071A54(void);
int rsin(int angle);
int rcos(int angle);

#endif
