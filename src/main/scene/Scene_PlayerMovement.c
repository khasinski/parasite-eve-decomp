/* Field player movement state handlers: idle, walk, run, turn-in-place and
 * the idle resync. All five are contiguous, compile under the default
 * profile and share the pad/analog and camera-angle globals. */
#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"
#include "pe1/gte_types.h"
#include "pe1/gte.h"
#include "pe1/render_camera.h"

void Scene_Init(BattleEntity *entity, int *actionState) {
    if (D_8009D1A0 & 2) {
        Scene_SyncEntityAction(entity, actionState);
        return;
    }
    if (*actionState != 0x15) {
        entity->motionX = 0;
        entity->motionY = 0;
        entity->motionZ = 0;
        Entity_SetActionMode(entity, 0x15);
        *actionState = 0x15;
    }
}

void Scene_UpdatePlayerEntity(BattleEntity *entity, int *actionState)
{
    GteVector movement;
    int speed;
    int diagonal;
    int facing;
    u32 pad;

    if (D_8009D1A0 & 2) {
        Scene_UpdateEntityFacing(entity, actionState);
        return;
    }
    if (*actionState != 22) {
        Entity_SetActionMode(entity, 22);
        *actionState = 22;
    }
    speed = Math_FixedMul(0x50000, entity->moveFactor);
    speed = Math_FixedMul(speed, entity->moveSpeed << 4);
    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        int angle;
        angle = ratan2(D_800BE9A7 - 128, D_800BE9A6 - 128) - 0x400;
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
    ApplyMatrixLV(&D_800BD000, &movement, (GteVector *)&entity->motionX);
    if (D_8009D2E8 & 0x10) {
        entity->facingAngle = (0x1000 - (u16)entity->facingAngle) & 0xFFF;
        entity->motionX = -entity->motionX;
    }
}

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
        angle = ratan2(D_800BE9A7 - 128, D_800BE9A6 - 128) - 0x400;
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
    ApplyMatrixLV(&D_800BD000, &movement, (GteVector *)&entity->motionX);
    if (D_8009D2E8 & 0x10) {
        entity->facingAngle = (0x1000 - (u16)entity->facingAngle) & 0xFFF;
        entity->motionX = -entity->motionX;
    }
}

void Scene_SyncEntityAction(BattleEntity *entity, int *actionState) {
    u8 mode;

    if (!(D_8009D1A0 & 2)) {
        if (*actionState != 0x15) {
            entity->motionX = 0;
            entity->motionY = 0;
            entity->motionZ = 0;
            Entity_SetActionMode(entity, 0x15);
            *actionState = 0x15;
        }
    } else {
        mode = ((Combatant *)entity->core)->actionMode12;
        if (*actionState != mode) {
            Entity_SetAction(entity, mode);
            entity->motionX = 0;
            entity->motionY = 0;
            entity->motionZ = 0;
            *actionState = ((Combatant *)entity->core)->actionMode12;
        }
    }
}

void Scene_UpdateEntityFacingFromPad(BattleEntity *entity) {
    int angle;
    Entity_SetAction(entity, 0x11);
    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        angle = ratan2(D_800BE9A7 - 128, D_800BE9A6 - 128) - 0x400;
        /* Keep the wrapped-angle correction based on the adjusted value. */
        asm volatile("" : "=r"(angle) : "0"(angle));
        if (angle < 0) angle += 0x1000;
        angle += D_800BD022;
    } else {
        if (D_8009D26C & FIELD_PAD_DOWN) {
            if (D_8009D26C & FIELD_PAD_LEFT) entity->facingAngle = 0x600;
            else if (D_8009D26C & FIELD_PAD_RIGHT) entity->facingAngle = 0xA00;
            else entity->facingAngle = 0x800;
        } else if (D_8009D26C & FIELD_PAD_UP) {
            if (D_8009D26C & FIELD_PAD_LEFT) entity->facingAngle = 0x200;
            else if (D_8009D26C & FIELD_PAD_RIGHT) entity->facingAngle = 0xE00;
            else entity->facingAngle = 0;
        } else if (D_8009D26C & FIELD_PAD_LEFT) entity->facingAngle = 0x400;
        else if (D_8009D26C & FIELD_PAD_RIGHT) entity->facingAngle = 0xC00;
        angle = entity->facingAngle + D_800BD020;
    }
    if (D_8009D254 && (D_8009D2E8 & 0x10)) {
        angle += 0x800;
        {
            Combatant *core = D_8009D254->core;
            angle += ((unsigned int)core->stateFlags >> 7) & 0xC00;
        }
    }
    angle &= 0xFFF;
    entity->facingAngle = angle;
    /* Retail reloads both the mode flags and the stored facing angle. */
    asm volatile("" : : : "memory");
    if (D_8009D2E8 & 0x10) entity->facingAngle = (0x1000 - (u16)entity->facingAngle) & 0xFFF;
}
