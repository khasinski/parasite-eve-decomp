#ifndef PE1_ROOM_PAIRED_GLOW_H
#define PE1_ROOM_PAIRED_GLOW_H

#include "common.h"
#include "pe1/room_fx.h"

/* The paired glow (src/overlays/room_lib/RoomLib_PairedGlow.c). */

/* Per-room data: the glow sprite's record and packet, and the colour table
 * the emitters step through. */
extern RoomFxSpritePacket g_RoomPairedGlowSprite;
extern void *g_RoomPairedGlowPacket;
extern unsigned char g_RoomPairedGlowColorTable[];

extern void *D_800B0E64;

int *func_800C2B10(int index);
void *func_8006E498(void *owner, int id);
void func_8006DF50(void *owner, int soundId, int arg2, int volume, int pan);
void func_800C4E50(void *params);
void func_800C4FC4(void *params, RoomSpriteMatrix *matrix, int mode);
void func_800C3134(void *table, int step, void *out);
void func_800C3238(int page);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_800C6D5C(void *packet, int arg1, int arg2);
int func_80077A64(int arg0, int arg1, int arg2, int arg3);
int func_80077AA4(int arg0, int arg1);
void func_800C6EC0(int arg0, int arg1);
void func_800C6ED8(int arg0);
void func_800C6EF8(void *packet);
void func_800C7098(void *packet, int arg1, int arg2, int arg3);
void func_800C6FA0(void *packet, int depth);
void func_800C71E4(void *packet, RoomSpriteMatrix *matrix);
void func_800C6F4C(void *packet);

#endif
