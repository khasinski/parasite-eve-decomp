/*
 * Glow orb module class, slot 1: plants the module's script and sets up the
 * orb's and the flashes' sprite blocks. The same function in room_m034,
 * room_m174 and room_m383, in front of the class methods that precede the
 * glow orb (RoomEffect_GlowOrb.c); the data is each room's own
 * (pe1/room_glow_orb.h).
 */
#include "pe1/room_glow_orb.h"

int RoomEffect_GlowOrbSetup(void) {
    *FieldEng_GetSlot() = g_RoomGlowOrbScript;
    g_RoomGlowOrbSprite.code = 0x20;
    g_RoomGlowOrbSprite.mode = 3;
    g_RoomGlowOrbFlashSprite.code = 0x2B;
    g_RoomGlowOrbFlashSprite.mode = 2;
    g_RoomGlowOrbFlashSprite.depth = 0x80;
    g_RoomGlowOrbSprite.offset = 0;
    g_RoomGlowOrbSprite.depth = 0;
    g_RoomGlowOrbSprite.zero = 0;
    g_RoomGlowOrbFlashSprite.offset = 0;
    g_RoomGlowOrbFlashSprite.r = 0x80;
    g_RoomGlowOrbFlashSprite.g = 0x80;
    g_RoomGlowOrbFlashSprite.b = 0x80;
    g_RoomGlowOrbFlashSprite.zero = 0;
    return 0;
}
