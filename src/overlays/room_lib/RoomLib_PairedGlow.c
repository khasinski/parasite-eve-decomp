/*
 * The paired glow: two paired light emitters (one announced with a sound)
 * scaled by a shared timer, and a glow sprite drawn over the owner that
 * grows and fades.
 *
 * room_m107, m111, m114, m118, m122 and scene_e11 to scene_e14 link the same
 * seventeen functions in this order, from the two class no-ops after
 * RoomLib_CloseTarget to the second particle tick, with one rotation seed
 * as their only read-only data. This unit is that object, compiled into the
 * five rooms. The four scenes link its first thirteen functions
 * (RoomLib_PairedGlowEmitters.inc, compiled by each scene) and keep their
 * own copies of the last four, which sit in a code segment linked 8 bytes
 * higher and whose glow sprite update branches differently; the emitter
 * init and transform templates are shared with those copies. The glow
 * sprite's record and packet and the emitters' colour table are room data,
 * named in each overlay's symbol file (pe1/room_paired_glow.h).
 */
#include "RoomLib_PairedGlowEmitters.inc"

/* Grows the glow sprite and fades it in for sixteen frames, then out. */
void RoomLib_UpdatePairedGlowSprite(void *unused, char *state, char *work) {
    *(unsigned short *)(work + 0x10) += 0xF;

    if (*(short *)(state + 2) < 0x10) {
        *(unsigned short *)(work + 0x12) += 8;
    } else {
        *(unsigned short *)(work + 0x12) -= 4;
    }

    *(unsigned short *)(work + 0xA) += 0x64;
    if (*(short *)(work + 0x12) <= 0) {
        state[1] = 2;
    }
}

#define ROOMLIB_INIT_PAIRED_EMITTER_NAME RoomLib_InitPairedEmitter
#include "RoomLib_InitPairedEmitter.inc"

#define ROOMLIB_TRANSFORM_PAIRED_SPRITE_FUNC RoomLib_TransformPairedEmitter
#define ROOMLIB_PAIRED_SPRITE_TABLE g_RoomPairedGlowColorTable
#include "RoomLib_TransformPairedRoomSprite.inc"

ROOMLIB_PARTICLE_TICK_A(RoomLib_TickPairedGlowB)
