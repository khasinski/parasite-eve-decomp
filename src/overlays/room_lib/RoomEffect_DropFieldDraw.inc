#include "pe1/gte.h"
#include "pe1/room_beam_pair.h"
#include "pe1/field_collision.h"
#ifndef ROOMEFFECT_DROP_FIELD_DRAW_FUNC
#error "ROOMEFFECT_DROP_FIELD_DRAW_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_DROP_FIELD_DRAW_SPIN
#error "ROOMEFFECT_DROP_FIELD_DRAW_SPIN must name the drop rotation"
#endif
#ifndef ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SPIN
#error "ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SPIN must name the shadow rotation"
#endif
#ifndef ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SCALE
#error "ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SCALE must name the shadow scale"
#endif
#ifndef ROOMEFFECT_DROP_FIELD_DRAW_SPRITE
#error "ROOMEFFECT_DROP_FIELD_DRAW_SPRITE must name the drop sprite block"
#endif

extern GteShortVector ROOMEFFECT_DROP_FIELD_DRAW_SPIN;
extern GteShortVector ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SPIN;
extern GteVector ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SCALE;
extern RoomDropSprite ROOMEFFECT_DROP_FIELD_DRAW_SPRITE;
extern PolygonVertex *D_8009D248;
extern u16 D_8009D1CC;
extern s16 D_800942EC;

/* Draw every live drop below the release height: the drop sprite at its
 * world position (retired once it leaves the walkable polygon; touching
 * the player flags the actor) and its shadow on the floor. */
void ROOMEFFECT_DROP_FIELD_DRAW_FUNC(void *object, void *slot, RoomDropField *field) {
    GteMatrix matrix;
    GteMatrix saved;
    GteMatrix turn;
    RoomDropActor *actor = func_800C2B50();
    GteShortVector spin = ROOMEFFECT_DROP_FIELD_DRAW_SPIN;
    GteShortVector shadowSpin = ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SPIN;
    GteShortVector rotation;
    GteShortVector point;
    GteVector scale;
    unsigned int i;

    func_800C2EAC(actor->shade);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x100);
    func_800C3238(2);
    rotation.x = 0x20;
    rotation.y = field->yaw;
    rotation.z = 0;
    RotMatrix(&rotation, &turn);
    turn.t[0] = 0;
    turn.t[1] = 0;
    turn.t[2] = 0;
    for (i = 0; i < 16; i++) {
        if (field->live[i] == 1 && field->drops[i].z < -0x28A) {
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
            matrix.t[0] = field->drops[i].x;
            matrix.t[1] = field->drops[i].y;
            matrix.t[2] = field->drops[i].z;
            gte_CompMatrix(&turn, &matrix, &matrix);
            gte_CompMatrix(&field->matrix, &matrix, &matrix);
            RotMatrix(&spin, &matrix);
            scale = (GteVector){ field->size[i], field->size[i], field->size[i] };
            Gte_ScaleMatrix(&matrix, &scale);
            ROOMEFFECT_DROP_FIELD_DRAW_SPRITE.depth = field->depth[i];
            saved = matrix;
            point.x = matrix.t[0];
            point.y = matrix.t[1];
            point.z = matrix.t[2];
            if (Geo_PointInPoly(matrix.t[0] << 16, matrix.t[2] << 16, D_8009D248, D_8009D1CC) == 0) {
                field->live[i] = 0;
            }
            if (func_800C6B90(&point, 200) != 0) {
                actor->touched = 1;
            }
            func_800C42A4(&ROOMEFFECT_DROP_FIELD_DRAW_SPRITE, &matrix, 1);
            matrix = saved;
            RotMatrix(&shadowSpin, &matrix);
            matrix.t[1] = D_800942EC;
            {
                GteVector shadow = ROOMEFFECT_DROP_FIELD_DRAW_SHADOW_SCALE;

                Gte_ScaleMatrix(&matrix, &shadow);
            }
            ROOMEFFECT_DROP_FIELD_DRAW_SPRITE.depth = field->depth[i] >> 1;
            func_800C42A4(&ROOMEFFECT_DROP_FIELD_DRAW_SPRITE, &matrix, 0);
        }
    }
}
