/*
 * Seeds of the homing projectile (RoomEffect_HomingProjectile.c): the bounce
 * spark's rotation and colour, the report fields and the ring's rotation and
 * colours. room_m141, room_m146, room_m153, room_m154, room_m328 and scene_e02
 * carry these 0x28 bytes in their data.
 */
#include "pe1/room_homing_model.h"

GteRotation g_RoomHomingSparkRotation = { 0x400, 0, 0, 1 };
RenderColor g_RoomHomingSparkColor = { 0x50, 0x20, 0, 0 };
s16 g_RoomHomingReportFields[5] = { 0, 1, 2, 5, 6 };
GteRotation g_RoomHomingRingRotation = { 0x400, 0, 0, 1 };
RenderColor g_RoomHomingRingInner = { 0x20, 0x10, 8, 0 };
RenderColor g_RoomHomingRingOuter = { 0, 0, 0, 0 };
