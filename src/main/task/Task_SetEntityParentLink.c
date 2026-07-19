typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
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
