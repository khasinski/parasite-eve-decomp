#include "pe1/asset_tim.h"
#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/pad_state.h"
#include "pe1/render_camera.h"

/* Field player movement state handlers: idle, walk, run, turn-in-place and
 * the idle resync. All five are contiguous, compile under the default
 * profile and share the pad/analog and camera-angle globals. */

void Scene_Init(BattleEntity *entity, int *actionState) {
    if (g_GameStateFlags & 2) {
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

    if (g_GameStateFlags & 2) {
        Scene_UpdateEntityFacing(entity, actionState);
        return;
    }
    if (*actionState != 22) {
        Entity_SetActionMode(entity, 22);
        *actionState = 22;
    }
    speed = Math_FixedMul(0x50000, entity->moveFactor);
    speed = Math_FixedMul(speed, entity->moveSpeed << 4);
    if ((g_PadPackets[0].header & 0xF000) == 0x7000) {
        int angle;
        angle = ratan2(g_PadPackets[0].leftY - 128, g_PadPackets[0].leftX - 128) - 0x400;
        PE1_COMPILER_LAUNDER(angle);
        if (angle < 0) angle += 0x1000;
        entity->facingAngle = g_CameraBaseAngleAnalog + angle;
        movement.x = Math_FixedMul(-speed, rsin(angle) << 4);
        movement.y = 0;
        movement.z = Math_FixedMul(-speed, rcos(angle) << 4);
    } else {
        diagonal = Math_FixedMul(speed, 0xB504);
        pad = g_FieldPadBits;
        entity->facingAngle = g_CameraBaseAngleDigital;
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
                entity->facingAngle = g_CameraBaseAngleDigital;
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
    if (D_8009CD10 && (D_8009CDA8 & FIELD_MOVE_CONFUSED)) {
        u32 stateFlags = ((Combatant *)D_8009CD10->core)->stateFlags;

        facing += 0x800;
        facing += (stateFlags >> 7) & 0xC00;
    }
    facing &= 0xFFF;
    entity->facingAngle = facing;
    ApplyMatrixLV(&g_CameraBasisMatrix, &movement, (GteVector *)&entity->motionX);
    if (D_8009CDA8 & FIELD_MOVE_CONFUSED) {
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

    speed = AYA_MOVE_SPEED_NORMAL;
    if (g_GameStateFlags & 2) {
        int flags = ((Combatant *)entity->core)->stateFlags;
        if ((flags & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_SLOW) {
            speed = AYA_MOVE_SPEED_STIFF;
        } else if (flags & AYA_HASTE) {
            speed = AYA_MOVE_SPEED_HASTE;
        }
        {
            u8 mode;
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
    if ((g_PadPackets[0].header & 0xF000) == 0x7000) {
        int angle;
        angle = ratan2(g_PadPackets[0].leftY - 128, g_PadPackets[0].leftX - 128) - 0x400;
        PE1_COMPILER_LAUNDER(angle);
        if (angle < 0) angle += 0x1000;
        entity->facingAngle = g_CameraBaseAngleAnalog + angle;
        movement.x = Math_FixedMul(-speed, rsin(angle) << 4);
        movement.y = 0;
        movement.z = Math_FixedMul(-speed, rcos(angle) << 4);
    } else {
        diagonal = Math_FixedMul(speed, 0xB504);
        pad = g_FieldPadBits;
        entity->facingAngle = g_CameraBaseAngleDigital;
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
                entity->facingAngle = g_CameraBaseAngleDigital;
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
    if (D_8009CD10 && (D_8009CDA8 & FIELD_MOVE_CONFUSED)) {
        u32 stateFlags = ((Combatant *)D_8009CD10->core)->stateFlags;

        facing += 0x800;
        facing += (stateFlags >> 7) & 0xC00;
    }
    facing &= 0xFFF;
    entity->facingAngle = facing;
    ApplyMatrixLV(&g_CameraBasisMatrix, &movement, (GteVector *)&entity->motionX);
    if (D_8009CDA8 & FIELD_MOVE_CONFUSED) {
        entity->facingAngle = (0x1000 - (u16)entity->facingAngle) & 0xFFF;
        entity->motionX = -entity->motionX;
    }
}

void Scene_SyncEntityAction(BattleEntity *entity, int *actionState) {
    u8 mode;

    if (!(g_GameStateFlags & 2)) {
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
    if ((g_PadPackets[0].header & 0xF000) == 0x7000) {
        angle = ratan2(g_PadPackets[0].leftY - 128, g_PadPackets[0].leftX - 128) - 0x400;
        /* Keep the wrapped-angle correction based on the adjusted value. */
        asm volatile("" : "=r"(angle) : "0"(angle));
        if (angle < 0) angle += 0x1000;
        angle += g_CameraBaseAngleAnalog;
    } else {
        if (g_FieldPadBits & FIELD_PAD_DOWN) {
            if (g_FieldPadBits & FIELD_PAD_LEFT) entity->facingAngle = 0x600;
            else if (g_FieldPadBits & FIELD_PAD_RIGHT) entity->facingAngle = 0xA00;
            else entity->facingAngle = 0x800;
        } else if (g_FieldPadBits & FIELD_PAD_UP) {
            if (g_FieldPadBits & FIELD_PAD_LEFT) entity->facingAngle = 0x200;
            else if (g_FieldPadBits & FIELD_PAD_RIGHT) entity->facingAngle = 0xE00;
            else entity->facingAngle = 0;
        } else if (g_FieldPadBits & FIELD_PAD_LEFT) entity->facingAngle = 0x400;
        else if (g_FieldPadBits & FIELD_PAD_RIGHT) entity->facingAngle = 0xC00;
        angle = entity->facingAngle + g_CameraBaseAngleDigital;
    }
    if (D_8009CD10 && (D_8009CDA8 & FIELD_MOVE_CONFUSED)) {
        angle += 0x800;
        {
            Combatant *core = D_8009CD10->core;
            angle += ((unsigned int)core->stateFlags >> 7) & 0xC00;
        }
    }
    angle &= 0xFFF;
    entity->facingAngle = angle;
    /* Retail reloads both the mode flags and the stored facing angle. */
    asm volatile("" : : : "memory");
    if (D_8009CDA8 & FIELD_MOVE_CONFUSED) entity->facingAngle = (0x1000 - (u16)entity->facingAngle) & 0xFFF;
}

/* Game-side TIM container helpers: upload a TIM's image and optional CLUT,
 * and return its CLUT/image rectangles and pixel pointers. No Psy-Q
 * signature of any SDK version matches these bytes; they are linked as the
 * last game object, directly before the LIBC veneers. */

int *Asset_LoadTimImage(TimFile *tim) {
    TimBlock *clut;
    TimBlock *image;
    int *pixels;

    clut = 0;
    if ((tim->flags & 8) != 0) {
        clut = &tim->first_block;
        image = (TimBlock *)&clut->bytes[clut->header.length];
    } else {
        image = &tim->first_block;
    }

    pixels = image->header.pixels;
    LoadImage(&image->header.rect, pixels);
    if (clut != 0) {
        LoadImage(&clut->header.rect, clut->header.pixels);
    }
    return pixels;
}

RECT *Asset_GetTimClutRect(TimFile *tim) {
    if (tim->flags & 8) {
        return &tim->first_block.header.rect;
    }
    return 0;
}

static inline TimBlock *ImageBlock(TimFile *tim) {
    TimBlock *image;

    if (tim->flags & 8) {
        TimBlock *clut = &tim->first_block;
        image = (TimBlock *)&clut->bytes[clut->header.length];
    } else {
        image = &tim->first_block;
    }
    return image;
}

RECT *Asset_GetTimImageRect(TimFile *tim) {
    return &ImageBlock(tim)->header.rect;
}

int *Asset_GetTimImagePixels(TimFile *tim) {
    return ImageBlock(tim)->header.pixels;
}

int *Asset_GetTimClutPixels(TimFile *tim) {
    if (tim->flags & 8) {
        return tim->first_block.header.pixels;
    }
    return 0;
}
