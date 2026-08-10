#include "common.h"
#include "pe1/battle.h"

typedef BattleEntity Entity;

extern Entity *g_FieldActorListHead;
extern Entity *g_CurrentEntity;

int Task_ClearEntityParentLink(void) {
    Entity *current;
    Entity *it;

    current = g_CurrentEntity;
    it = g_FieldActorListHead;
    current->parent = 0;
    current->entityFlags &= 0xFF9FFFFF;

    if (it != 0) {
        do {
            if (it != current) {
                if (it->parent == current->parent) {
                    return 1;
                }
            }
            it = it->next;
        } while (it != 0);
    }

    {
        Entity *tail_current;
        int clear_mask;
        tail_current = g_CurrentEntity;
        clear_mask = 0xFFEFFFFF;
        tail_current->parent->entityFlags &= clear_mask;
    }
    return 1;
}
