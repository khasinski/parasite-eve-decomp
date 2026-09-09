#include "common.h"
#define NULL ((void *)0)
#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead[];
#define g_FieldActorListHead (g_FieldActorListHead[0])
extern FieldActor *g_PlayerEntity[];
#define g_PlayerEntity (g_PlayerEntity[0])
extern FieldActor *g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])

/* Script op: attach the current entity to the actor named by
 * (args[0], args[1]) as its parent (pos/rot follow when flags & 0x400000). */
s32 Task_SetEntityParentLink(s32 *args[]) {
    s32 key;
    FieldActor *node;

    key = *args[0];
    if (key == 0) {
        FieldActor *tmp;

        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto done;
        }
        node = tmp;
        goto link;
    }
    {
        u32 k = key;
        node = g_FieldActorListHead;
        if (node != NULL) {
loop:
        if ((node->type_id != k) || (node->sub_id != *args[1]) || (node->flags & 0x10)) {
            node = node->next;
            if (node != NULL) {
                goto loop;
            }
        }
            if (node != NULL) {
                goto link;
            }
        }
    }
done:
    return 1;
link:
    {
        FieldActor *cur = g_CurrentEntity;
        cur->parent = node;
        node->flags |= 0x100000;
        cur->flags |= 0x600000;
    }
    return 1;
}

int Task_ClearEntityParentLink(void) {
    FieldActor *current;
    FieldActor *it;

    current = g_CurrentEntity;
    it = g_FieldActorListHead;
    current->parent = 0;
    current->flags &= 0xFF9FFFFF;

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
        FieldActor *tail_current;
        int clear_mask;
        tail_current = g_CurrentEntity;
        clear_mask = 0xFFEFFFFF;
        tail_current->parent->flags &= clear_mask;
    }
    return 1;
}

int Entity_ClearParentLink(void) {
    int clear_current = 0xFFEFFFFF;
    FieldActor *current = g_CurrentEntity;
    FieldActor *it = g_FieldActorListHead;

    current->flags &= clear_current;

    if (it != 0) {
        int clear_child = 0xFF9FFFFF;

        do {
        if (it->parent == current) {
            it->parent = 0;
                it->flags &= clear_child;
        }
            it = it->next;
        } while (it != 0);
    }

    return 1;
}
