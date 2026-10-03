#include "pe1/gte.h"
#include "pe1/room_glow_orb.h"
#ifndef ROOMEFFECT_GLOW_ORB_DRAW_FUNC
#error "ROOMEFFECT_GLOW_ORB_DRAW_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_GLOW_ORB_DRAW_SPRITE
#error "ROOMEFFECT_GLOW_ORB_DRAW_SPRITE must name the sprite parameter block"
#endif

extern RoomGlowOrbSprite ROOMEFFECT_GLOW_ORB_DRAW_SPRITE;

/* Draw the glow orb: two scaled halo layers at the orb, a touch test
 * against the player that flags the pickup, then three trailing sparks
 * stepping back along the orb's offset. The GNU constructor assignments
 * clear a temporary with memset and copy it, and the second scale lives in
 * a nested block so it takes the first temporary's freed stack slot. */
void ROOMEFFECT_GLOW_ORB_DRAW_FUNC(RoomGlowOrbObject *object, u8 *slot, RoomGlowOrb *orb) {
    GteMatrix matrix;
    GteShortVector point;

    func_800C2EAC(0);
    func_800C3098(0x10);
    func_800C2FF0(0x40, 0x20);
    func_800C3238(2);
    if (orb->state == 1) {
        GteVector scale;

        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.r = 0x10;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.g = 0x10;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.b = 0x20;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.depth = orb->depth;
        matrix = orb->matrix;
        /* Retail subtracts offset.x on all three axes for the inner halo. */
        matrix.t[0] = orb->x - orb->offset.x;
        matrix.t[1] = orb->y - orb->offset.x;
        matrix.t[2] = orb->z - orb->offset.x;
        scale = (GteVector){ orb->size >> 1, 0x251, 0x251 };
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C42A4(&ROOMEFFECT_GLOW_ORB_DRAW_SPRITE, &matrix, 0);

        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.r = 0x20;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.g = 0x20;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.b = 0x40;
        matrix = orb->matrix;
        matrix.t[0] = orb->x;
        matrix.t[1] = orb->y;
        matrix.t[2] = orb->z;
        {
            GteVector size;

            size = (GteVector){ orb->size, 0x448, 0x448 };
            Gte_ScaleMatrix(&matrix, &size);
            func_800C42A4(&ROOMEFFECT_GLOW_ORB_DRAW_SPRITE, &matrix, 0);
        }

        point.x = D_8009D254->x.part.integer;
        point.y = D_8009D254->y.part.integer;
        point.z = D_8009D254->z.part.integer;
        if (func_800C61A8(&point, &matrix) != 0 && FieldEng_GetStatus(object) == 3) {
            D_8009D254->state->flags |= 0x4000;
            if (object->link != 0) {
                **object->link |= 0x80000000;
            }
            slot[1] = 2;
        }

        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.r = 0x78;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.g = 0xF0;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.b = 0x78;
        matrix.t[1] = orb->y - 0x100;
        func_800C42A4(&ROOMEFFECT_GLOW_ORB_DRAW_SPRITE, &matrix, 0);

        matrix.t[0] = orb->x - orb->offset.x;
        matrix.t[1] = orb->y - orb->offset.y;
        matrix.t[2] = orb->z - orb->offset.z;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.r = 0x3C;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.g = 0x78;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.b = 0x3C;
        matrix.t[1] = orb->y - 0x100;
        func_800C42A4(&ROOMEFFECT_GLOW_ORB_DRAW_SPRITE, &matrix, 0);

        matrix.t[0] = orb->x - orb->offset.x * 2;
        matrix.t[1] = orb->y - orb->offset.y * 2;
        matrix.t[2] = orb->z - orb->offset.z * 2;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.r = 0x1E;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.g = 0x3C;
        ROOMEFFECT_GLOW_ORB_DRAW_SPRITE.b = 0x1E;
        matrix.t[1] = orb->y - 0x100;
        func_800C42A4(&ROOMEFFECT_GLOW_ORB_DRAW_SPRITE, &matrix, 0);
    }
}
