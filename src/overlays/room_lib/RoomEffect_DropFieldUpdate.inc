#include "pe1/gte.h"
#include "pe1/room_beam_pair.h"
#ifndef ROOMEFFECT_DROP_FIELD_UPDATE_FUNC
#error "ROOMEFFECT_DROP_FIELD_UPDATE_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_DROP_FIELD_UPDATE_CALLBACK
#error "ROOMEFFECT_DROP_FIELD_UPDATE_CALLBACK must name the spawned callback"
#endif
#ifndef ROOMEFFECT_DROP_FIELD_UPDATE_DATA
#error "ROOMEFFECT_DROP_FIELD_UPDATE_DATA must name the spawned data"
#endif

extern u8 ROOMEFFECT_DROP_FIELD_UPDATE_CALLBACK[];
extern u8 ROOMEFFECT_DROP_FIELD_UPDATE_DATA[];

/* Sink every drop; when the script step reaches a drop and flag 7 is set,
 * retire it and, for the first one, spawn the splash at its world
 * position (drop offset turned by the field yaw and the field matrix). */
void ROOMEFFECT_DROP_FIELD_UPDATE_FUNC(void *object, RoomDropSlot *slot, RoomDropField *field) {
    GteMatrix matrix;
    GteMatrix turn;
    GteShortVector rotation;
    RoomDropPoint *splash;
    unsigned int i;

    for (i = 0; i < 16; i++) {
        field->drops[i].z -= 0x96;
        if (slot->step == field->first_step + i && *func_800C2B28(7) == 1) {
            field->live[i] = 0;
            if (++field->released == 1) {
                rotation.x = 0x20;
                rotation.y = field->yaw;
                rotation.z = 0;
                RotMatrix(&rotation, &turn);
                turn.t[0] = 0;
                turn.t[1] = 0;
                turn.t[2] = 0;
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
                splash = func_800C2B90(object, 1, ROOMEFFECT_DROP_FIELD_UPDATE_CALLBACK,
                                       ROOMEFFECT_DROP_FIELD_UPDATE_DATA);
                if (splash != 0) {
                    splash->x = matrix.t[0];
                    splash->y = matrix.t[1];
                    splash->z = matrix.t[2];
                }
            }
        }
    }
    if (slot->step >= 0x79) {
        slot->command = 2;
    }
}
