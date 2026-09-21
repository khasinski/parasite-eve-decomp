#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"
#include "pe1/gte_types.h"

void Scene_UpdateEntityFacingFromPad(BattleEntity *entity) {
    int angle;
    Entity_SetAction(entity, 0x11);
    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        angle = Gte_Atan2(D_800BE9A7 - 128, D_800BE9A6 - 128) - 0x400;
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
