#include "pe1/entity_animation.h"
/* MASPSX_FLAGS: --expand-div */


void Akao_LoadVoiceBank(BattleEntity *entity);

void Entity_AdvanceAnim(BattleEntity *entity) {
    int flags;
    int new_flags;
    int old_frame;
    register int step;
    int limit;
    int next_frame;
    int end_frame;

    if (entity != 0) {
        if (entity->parent != 0 && !(entity->parent->entityFlags & 0x800000)) {
            Entity_AdvanceAnim(entity->parent);
        }

        Akao_LoadVoiceBank(entity);
        new_flags = (entity->entityFlags & -9) | 0x800000;
        entity->entityFlags = new_flags;
        old_frame = entity->animFrame;
        entity->animPrev.fixed = old_frame;
        flags = entity->entityFlags;

        if (!(flags & 0x100)) {
            if (flags & 0x200) {
                if (((u32)old_frame >> 16) == entity->animStopFrame) {
                    return;
                }
            }

            if (flags & 0x200000) {
                Entity_SetAction(entity, entity->parent->actionMode);
                entity->animFrame = entity->parent->animFrame;
                return;
            }

            step = entity->animStep;
            limit = entity->animStopFrame;
            next_frame = old_frame + step;
            end_frame = limit << 16;

            if (flags & 0x200) {
                if (!(old_frame < end_frame)) {
                    goto check_reverse_crossing;
                }
                if (end_frame < next_frame) {
                    entity->animFrame = end_frame;
                    return;
                }
check_reverse_crossing:
                if (end_frame < old_frame) {
                    if (next_frame < end_frame) {
                        entity->animFrame = end_frame;
                        return;
                    }
                }
            }

            if (entity->animLastFrame < (next_frame >> 16)) {
                limit = (next_frame >> 16) % (entity->animLastFrame + 1);
                old_frame = 0;
                entity->entityFlags |= 8;
                next_frame = limit << 16;
            } else if (next_frame < 0) {
                old_frame = entity->animLastFrame << 16;
                next_frame += (entity->animLastFrame + 1) << 16;
                entity->entityFlags |= 8;
            }

            if (entity->entityFlags & 0x200) {
                if (end_frame < old_frame) {
                    goto final_reverse_crossing;
                }
                if (end_frame < next_frame) {
                    entity->animFrame = end_frame;
                    return;
                }
final_reverse_crossing:
                if (old_frame < end_frame) {
                    goto store_next;
                }
                if (next_frame < end_frame) {
                    entity->animFrame = end_frame;
                    return;
                }
            }

store_next:
            entity->animFrame = next_frame;
        }
    }
}



extern BattleEntity *g_FieldActorListHead;

void Entity_SetActionMode(BattleEntity *arg0, int arg1) {
    BattleEntity *entity;
    int mode;
    unsigned int idx;
    EntityActionTable *base;
    BattleEntity *it;
    void **table;
    void *action;

    entity = arg0;
    mode = arg1;
    idx = entity->modelIndex;
    base = D_800B0E98;
    table = base[idx].actions;
    action = table[(unsigned short)mode];
    entity->actionMode = mode;
    entity->animFrame = 0;
    entity->animPrev.fixed = 0;
    entity->actionData = action;
    entity->entityFlags &= -0x201;
    entity->animLastFrame = *((u8 *)entity->actionData + 2) - 1;

    if (entity->entityFlags & 0x100000) {
        it = g_FieldActorListHead;
        if (it != 0) {
            do {
                if (it->parent == entity && (it->entityFlags & 0x200000)) {
                    Entity_SetActionMode(it, (unsigned short)mode);
                }
                it = it->next;
            } while (it != 0);
        }
    }
}

void Entity_SetAction(BattleEntity *arg0, int arg1) {
    BattleEntity *entity;
    int raw_mode;
    unsigned int mode;
    unsigned int idx;
    EntityActionTable *base;
    BattleEntity *it;
    void **table;
    void *action;

    entity = arg0;
    raw_mode = arg1;
    mode = (unsigned short)raw_mode;
    if (entity->actionMode != mode) {
        idx = entity->modelIndex;
        base = D_800B0E98;
        table = base[idx].actions;
        entity->animFrame = 0;
        entity->animPrev.fixed = 0;
        entity->actionMode = raw_mode;
        action = table[mode];
        entity->actionData = action;
        entity->animLastFrame = *((u8 *)action + 2) - 1;
    }

    entity->entityFlags &= -0x201;
    if (entity->entityFlags & 0x100000) {
        it = g_FieldActorListHead;
        if (it != 0) {
            do {
                if (it->parent == entity && (it->entityFlags & 0x200000)) {
                    Entity_SetAction(it, (unsigned short)raw_mode);
                }
                it = it->next;
            } while (it != 0);
        }
    }
}
