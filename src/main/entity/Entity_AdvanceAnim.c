#include "common.h"
#include "pe1/battle.h"
/* MASPSX_FLAGS: --expand-div */

typedef BattleEntity Entity;

void Entity_AdvanceAnim(Entity *entity);
void Entity_SetAction(Entity *arg0, int arg1);
void Akao_LoadVoiceBank(Entity *entity);

void Entity_AdvanceAnim(Entity *entity) {
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
        flags = *(volatile u32 *)&entity->entityFlags;

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
