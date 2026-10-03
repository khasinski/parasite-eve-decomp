#include "pe1/gte.h"
#include "pe1/room_beam_pair.h"
#ifndef ROOMEFFECT_BEAM_PAIR_DRAW_FUNC
#error "ROOMEFFECT_BEAM_PAIR_DRAW_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_BEAM_PAIR_DRAW_SPIN
#error "ROOMEFFECT_BEAM_PAIR_DRAW_SPIN must name the beam rotation"
#endif
#ifndef ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES
#error "ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES must name the two sprite blocks"
#endif

extern GteShortVector ROOMEFFECT_BEAM_PAIR_DRAW_SPIN;
extern RoomBeamSprite ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES[2];

/* Draw both beam halves at the script actor's anchor, offset 100 up and
 * 100 back, spun by the room rotation and scaled uniformly. */
void ROOMEFFECT_BEAM_PAIR_DRAW_FUNC(void *object, void *slot, RoomBeamPair *beam) {
    GteMatrix matrix;
    GteMatrix second;
    RoomBeamPairActor **actor = func_800C2B50();
    GteShortVector spin = ROOMEFFECT_BEAM_PAIR_DRAW_SPIN;
    GteVector scale;

    beam->matrix = *(GteMatrix *)((*actor)->model + 0xA0);
    ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES[0].depth = beam->brightness;
    ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES[1].depth = beam->brightness;
    func_800C3238(2);
    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;
    matrix.t[0] = 0;
    matrix.t[1] = 100;
    matrix.t[2] = -100;
    gte_CompMatrix(&beam->matrix, &matrix, &matrix);
    RotMatrix(&spin, &matrix);
    scale = (GteVector){ beam->scale, beam->scale, beam->scale };
    Gte_ScaleMatrix(&matrix, &scale);
    second = matrix;
    func_800C4FC4(&ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES[0], &matrix, 1);
    func_800C4FC4(&ROOMEFFECT_BEAM_PAIR_DRAW_SPRITES[1], &second, 1);
}
