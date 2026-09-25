#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"
#include "pe1/gte_types.h"
#include "pe1/gte.h"
#include "pe1/render_camera.h"

void Scene_UpdateEntityFacing(BattleEntity *entity, int *actionState)
{
    GteVector movement;
    int speed;
    int diagonal;
    int facing;
    u32 pad;

    speed = 0x168000;
    if (D_8009D1A0 & 2) {
        int flags = ((Combatant *)entity->core)->stateFlags;
        if ((flags & 0xC0) == 0x40) {
            speed = 0xB4000;
        } else if (flags & 0x100) {
            speed = 0x21C000;
        }
        {
            u8 mode;
            /* The action-mode lookup reads core again after the flag check. */
            PE1_COMPILER_MEMORY_BARRIER();
            mode = (u8)((Combatant *)entity->core)->actionMode13;
            if (*actionState != mode) {
                Entity_SetActionMode(entity, mode);
                *actionState = (u8)((Combatant *)entity->core)->actionMode13;
            }
        }
    } else if (*actionState != 23) {
        Entity_SetActionMode(entity, 23);
        *actionState = 23;
    }
    speed = Math_FixedMul(speed, entity->moveFactor);
    speed = Math_FixedMul(speed, entity->moveSpeed << 4);
    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        int angle;
        angle = Gte_Atan2(D_800BE9A7 - 128, D_800BE9A6 - 128) - 0x400;
        PE1_COMPILER_LAUNDER(angle);
        if (angle < 0) angle += 0x1000;
        entity->facingAngle = D_800BD022 + angle;
        movement.x = Math_FixedMul(-speed, rsin(angle) << 4);
        movement.y = 0;
        movement.z = Math_FixedMul(-speed, rcos(angle) << 4);
    } else {
        diagonal = Math_FixedMul(speed, 0xB504);
        pad = D_8009D26C;
        entity->facingAngle = D_800BD020;
        if (pad & FIELD_PAD_DOWN) {
            if (pad & FIELD_PAD_LEFT) {
                entity->facingAngle += 0x600;
                movement.x = -diagonal;
                movement.y = 0;
                movement.z = diagonal;
            } else if (pad & FIELD_PAD_RIGHT) {
                entity->facingAngle += 0xA00;
                movement.x = diagonal;
                movement.y = 0;
                movement.z = diagonal;
            } else {
                entity->facingAngle += 0x800;
                movement.x = 0;
                movement.y = 0;
                movement.z = speed;
            }
        } else if (pad & FIELD_PAD_UP) {
            if (pad & FIELD_PAD_LEFT) {
                entity->facingAngle += 0x200;
                movement.x = -diagonal;
                movement.y = 0;
                movement.z = -diagonal;
            } else if (pad & FIELD_PAD_RIGHT) {
                entity->facingAngle += 0xE00;
                movement.x = diagonal;
                movement.y = 0;
                movement.z = -diagonal;
            } else {
                entity->facingAngle = D_800BD020;
                movement.x = 0;
                movement.y = 0;
                movement.z = -speed;
            }
        } else if (pad & FIELD_PAD_LEFT) {
            entity->facingAngle += 0x400;
            movement.x = -speed;
            movement.y = 0;
            movement.z = 0;
        } else if (pad & FIELD_PAD_RIGHT) {
            entity->facingAngle += 0xC00;
            movement.x = speed;
            movement.y = 0;
            movement.z = 0;
        }
    }
    facing = entity->facingAngle;
    if (D_8009D254 && (D_8009D2E8 & 0x10)) {
        facing += 0x800;
        facing += ((u32)((Combatant *)D_8009D254->core)->stateFlags >> 7) & 0xC00;
    }
    facing &= 0xFFF;
    entity->facingAngle = facing;
    Gte_RotateVec(&D_800BD000, &movement, (GteVector *)&entity->motionX);
    if (D_8009D2E8 & 0x10) {
        entity->facingAngle = (0x1000 - (u16)entity->facingAngle) & 0xFFF;
        entity->motionX = -entity->motionX;
    }
}
