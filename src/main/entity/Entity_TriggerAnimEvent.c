#include "pe1/battle_entity_anim.h"

/* Six register pins preserve retail register
 * lifetimes and mask/address scheduling with stock GCC and maspsx. */

int Entity_TriggerAnimEvent(BattleEntity *input, u8 slot)
{
    register BattleEntity *entity asm("$6") = input;
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
            actor->animLastFrame < nextFrame && !(core->flags & 0x40000000)) {
            Entity_SetActionMode(actor, (u16)(s8)core->baseMode);
            actor->animStep = 0x10000;
            if (slot < 3) {
                result = 1;
                core->active->state = 4;
            }
        }
        if (core->kind == 0 && (core->flags & 0x6000)) {
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
        register unsigned offset asm("$7") = index * sizeof(EnemyActionEffect);
        if (((EntityAnimEventCore *)((u8 *)core + offset))->records[0].state == 0) {
            unsigned selectMask = 0xFF1FFFFF;
            unsigned clearMask = 0xBFFFFFFF;
            unsigned signMask = 0x7FFFFFFF;
            register unsigned recordOffset asm("$2");
            recordOffset = offset + PE1_OFFSETOF(EntityAnimEventCore, records);
            core->active = (EnemyActionEffect *)((u8 *)core + recordOffset);
            core->flags = (core->flags & selectMask) | ((index & 7) << 21);
            core->flags &= clearMask;
            core->flags &= signMask;
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
