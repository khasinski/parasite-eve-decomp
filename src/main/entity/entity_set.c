#include "common.h"
#include "pe1/battle.h"

typedef BattleEntity Entity;

extern Entity *g_FieldActorListHead;
extern void *D_800B0E98;

void Entity_SetActionMode(Entity *arg0, int arg1) {
    Entity *entity;
    int mode;
    unsigned int idx;
    void *base;
    Entity *it;
    void **table;
    void *action;

    entity = arg0;
    mode = arg1;
    idx = entity->modelIndex;
    base = &D_800B0E98;
    table = (void **)((char *)base + ((idx * 3) << 6));
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

void Entity_SetAction(Entity *arg0, int arg1) {
    Entity *entity;
    int raw_mode;
    unsigned int mode;
    unsigned int idx;
    void *base;
    Entity *it;
    void **table;
    void *action;

    entity = arg0;
    raw_mode = arg1;
    mode = (unsigned short)raw_mode;
    if (entity->actionMode != mode) {
        idx = entity->modelIndex;
        base = &D_800B0E98;
        table = (void **)((char *)base + ((idx * 3) << 6));
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
