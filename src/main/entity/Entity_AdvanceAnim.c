/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Entity Entity;

struct Entity {
    char pad0[0x0E];
    u8 field0E;
    u8 field0F;
    char pad10[0x12 - 0x10];
    u16 field12;
    int frame14;
    int frame18;
    int step1C;
    char pad20[0x98 - 0x20];
    u32 flags98;
    char pad9C[0x18C - 0x9C];
    Entity *parent18C;
};

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
        if (entity->parent18C != 0 && !(entity->parent18C->flags98 & 0x800000)) {
            Entity_AdvanceAnim(entity->parent18C);
        }

        Akao_LoadVoiceBank(entity);
        new_flags = (entity->flags98 & -9) | 0x800000;
        entity->flags98 = new_flags;
        old_frame = entity->frame14;
        entity->frame18 = old_frame;
        flags = *(volatile u32 *)&entity->flags98;

        if (!(flags & 0x100)) {
            if (flags & 0x200) {
                if (((u32)old_frame >> 16) == entity->field12) {
                    return;
                }
            }

            if (flags & 0x200000) {
                Entity_SetAction(entity, entity->parent18C->field0E);
                entity->frame14 = entity->parent18C->frame14;
                return;
            }

            step = entity->step1C;
            limit = entity->field12;
            next_frame = old_frame + step;
            end_frame = limit << 16;

            if (flags & 0x200) {
                if (!(old_frame < end_frame)) {
                    goto check_reverse_crossing;
                }
                if (end_frame < next_frame) {
                    entity->frame14 = end_frame;
                    return;
                }
check_reverse_crossing:
                if (end_frame < old_frame) {
                    if (next_frame < end_frame) {
                        entity->frame14 = end_frame;
                        return;
                    }
                }
            }

            if (entity->field0F < (next_frame >> 16)) {
                limit = (next_frame >> 16) % (entity->field0F + 1);
                old_frame = 0;
                entity->flags98 |= 8;
                next_frame = limit << 16;
            } else if (next_frame < 0) {
                old_frame = entity->field0F << 16;
                next_frame += (entity->field0F + 1) << 16;
                entity->flags98 |= 8;
            }

            if (entity->flags98 & 0x200) {
                if (end_frame < old_frame) {
                    goto final_reverse_crossing;
                }
                if (end_frame < next_frame) {
                    entity->frame14 = end_frame;
                    return;
                }
final_reverse_crossing:
                if (old_frame < end_frame) {
                    goto store_next;
                }
                if (next_frame < end_frame) {
                    entity->frame14 = end_frame;
                    return;
                }
            }

store_next:
            entity->frame14 = next_frame;
        }
    }
}
