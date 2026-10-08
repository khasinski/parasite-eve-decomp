#include "pe1/battle_entity_anim.h"

/* Three register pins preserve the initial core, retained core and parent
 * lifetimes with stock GCC and maspsx. */

int Entity_TriggerAnimEvent(BattleEntity *entity, u8 slot)
{
    int result = 0;
    register EntityAnimEventCore *initial asm("$3") = entity->core;
    register EntityAnimEventCore *core asm("$16");
    BattleEntity *actor;
    u32 nextFrame;
    if (!initial || !(D_8009D1A0 & 2))
        return 1;
    core = initial;
    if (core->kind == 2 || (actor = entity, core->kind == 4)) {
        register BattleEntity *parent asm("$4") = entity->parent;
        actor = parent;
        if (!parent)
            actor = entity;
    } else {
        actor = entity;
    }
    if (slot >= 6)
        return result;
    if ((u32)actor->animPrev.fixed > actor->animFrame)
        nextFrame = ((u32)actor->animFrame >> 16) + actor->animLastFrame + 1;
    else
        nextFrame = (u32)actor->animFrame >> 16;
    switch (actor->actionMode) {
    case 0:
    case 1:
        break;
    case 6: case 8: case 10: case 12: case 14:
        if (actor->animLastFrame >= actor->animPrev.parts.integer &&
            actor->animLastFrame < nextFrame) {
            Entity_SetActionMode(actor, core->active->exitMode);
            Asset_Find08w(core->assetId, 0,
                         actor->renderObject.target_x,
                         actor->renderObject.target_y,
                         actor->renderObject.target_z);
            actor->animStep = core->active->exitStep;
            core->active->state = 1;
        }
        break;
    case 7: case 9: case 11: case 13: case 15:
        if (actor->animLastFrame >= actor->animPrev.parts.integer &&
            actor->animLastFrame < nextFrame && !(core->flags.raw & 0x40000000)) {
            Entity_SetActionMode(actor, (u16)(s8)core->baseMode);
            actor->animStep = 0x10000;
            if (slot < 3) {
                result = 1;
                core->active->state = 4;
            }
        }
        if (core->kind == 0 && (core->flags.raw & 0x6000)) {
            if (core->active->state < 2)
                core->active->state = 4;
            result = 2;
        }
        if (slot < 3 && (core->active->category == 1 || core->active->category == 3)) {
            if (core->active->frame < ((u32)actor->animFrame >> 16) &&
                core->active->frame >= actor->animPrev.parts.integer)
                core->active->state = 3;
        }
        break;
    default: {
        unsigned index = slot;
        if (core->records[index].state == 0) {
            core->active = &core->records[index];
            core->flags.bits.selectedRecord = index;
            core->flags.bits.bit30 = 0;
            core->flags.bits.bit31 = 0;
            Entity_SetActionMode(actor, core->active->enterMode);
            actor->animStep = core->active->enterStep;
        }
        break;
    }
    }
    if (core->active->state == 4 || core->records[slot].state == 4) {
        Entity_SetActionMode(actor, (u16)(s8)core->baseMode);
        actor->animStep = 0x10000;
        result = 1;
        core->active->state = 0;
        core->active = 0;
    }
    return result;
}
