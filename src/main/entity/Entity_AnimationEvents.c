/* Script setup and execution share six inline animation-event records. */
#include "common.h"
#include "pe1/task_anim.h"

void Task_SetObjAnimEntry12(TaskAnimObj *obj, u8 index, int arg2, int arg3, u8 arg4, u16 arg5,
                   u8 arg6, u8 arg7, u8 arg8, u8 arg9, u8 arg10, u8 arg11) {
    EnemyActionEffect *dst = &obj->core->records[index];

    dst->state = 0;
    dst->effectType = arg2;
    dst->enterMode = arg3;
    dst->exitMode = arg4;
    dst->power = arg5;
    dst->category = arg10;
    dst->frame = arg11;

    obj->core->parameters[index][0] = arg6;
    obj->core->parameters[index][1] = arg7;
    obj->core->parameters[index][2] = arg8;
    obj->core->parameters[index][3] = arg9;
}

void Task_SetObjAnimEntry5(TaskAnimObj *obj, int index, int arg2, int arg3, u8 arg4, u16 arg5) {
    EnemyActionEffect *dst = &obj->core->records[(u8)index];

    dst->state = 0;
    dst->effectType = arg2;
    dst->enterMode = arg3;
    dst->exitMode = arg4;
    dst->power = arg5;
}

void Battle_SetEntryCoords(TaskAnimObj *arg0, unsigned char arg1, int arg2, int arg3) {
    EnemyActionEffect *entry = &arg0->core->records[arg1];

    entry->enterStep = arg2;
    entry->exitStep = arg3;
}


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
