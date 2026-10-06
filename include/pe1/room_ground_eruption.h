#ifndef PE1_ROOM_GROUND_ERUPTION_H
#define PE1_ROOM_GROUND_ERUPTION_H

#include "common.h"
#include "pe1/room_fx.h"

/* The ground eruption (src/overlays/room_lib/RoomEffect_GroundEruption.c). */

/* Per-room data: the four sprite packets the eruption draws with, named in
 * each overlay's symbol file. The glow is the flash at the owner's feet,
 * the debris the six flung particles, the shadow the floor sprite under
 * the rising column and the ring the sixteen-point shock ring. */
extern RoomFxSpritePacket g_RoomEruptionGlowPacket;
extern RoomFxSpritePacket g_RoomEruptionDebrisPacket;
extern RoomFxSpritePacket g_RoomEruptionShadowPacket;
extern RoomFxSpritePacket g_RoomEruptionRingPacket;
/* The debris packet's depth as its own symbol: the debris draw stores the
 * floor shadow's depth through a separate address, and a store through the
 * packet would let the compiler keep the packet's address in a register
 * across the loop. */
extern s16 g_RoomEruptionDebrisDepth;

int *func_800C2B28(int index);
int func_800C66C8(void *object, int message, void *state);
void func_800C4E50(void *params);
void func_800C4FC4(void *params, RoomSpriteMatrix *matrix, int mode);
void func_80071A44(void *dst, int value, int size);

#endif
