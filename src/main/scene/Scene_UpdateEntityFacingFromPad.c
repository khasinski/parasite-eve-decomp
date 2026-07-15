typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#include "pe1/field_actor.h"
#include "pe1/field_movement.h"

extern u16 D_800BE9A0;
extern u8 D_800BE9A6;
extern u8 D_800BE9A7;
extern u32 D_8009D26C;
extern u16 D_800BD020;
extern u16 D_800BD022;
extern void *D_8009D254;
extern u32 D_8009D2E8;

void Entity_SetAction(FieldActor *entity, int action);
int Gte_Atan2(int y, int x);

void Scene_UpdateEntityFacingFromPad(FieldActor *entity) {
    int angle;
    int camera_angle;
    u32 pad;

    Entity_SetAction(entity, 0x11);

    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        angle = Gte_Atan2(D_800BE9A7 - 0x80, D_800BE9A6 - 0x80) - 0x400;
        if (angle < 0) {
            angle += 0x1000;
        }
        camera_angle = D_800BD022;
    } else {
        pad = D_8009D26C;
        if (pad & FIELD_PAD_DOWN) {
            if (pad & FIELD_PAD_LEFT) {
                angle = 0x600;
            } else if (pad & FIELD_PAD_RIGHT) {
                angle = 0xA00;
            } else {
                angle = 0x800;
            }
            entity->rot_y = angle;
        } else if (pad & FIELD_PAD_UP) {
            if (pad & FIELD_PAD_LEFT) {
                angle = 0x200;
            } else if (pad & FIELD_PAD_RIGHT) {
                angle = 0xE00;
            } else {
                entity->rot_y = 0;
            }
        } else {
            if (pad & FIELD_PAD_LEFT) {
                angle = 0x400;
                entity->rot_y = angle;
            } else if (pad & FIELD_PAD_RIGHT) {
                angle = 0xC00;
                entity->rot_y = angle;
            }
        }

        angle = (short)entity->rot_y;
        camera_angle = D_800BD020;
    }

    angle += camera_angle;
    if (D_8009D254 != 0 && (D_8009D2E8 & 0x10) != 0) {
        angle += 0x800;
        angle += ((*(int *)(*(int *)D_8009D254 + 0x4C) >> 7) & 0xC00);
    }

    entity->rot_y = angle & 0xFFF;

    if (D_8009D2E8 & 0x10) {
        entity->rot_y = (0x1000 - entity->rot_y) & 0xFFF;
    }
}
