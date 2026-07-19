typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead[];
#define g_FieldActorListHead (g_FieldActorListHead[0])
extern FieldActor *g_PlayerEntity[];
#define g_PlayerEntity (g_PlayerEntity[0])
extern FieldActor *g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])

/* Script op: look up an actor by (type id, sub id) script args and read one
 * of its fields into the out arg. args = script argument pointer array:
 * args[0]=field selector, args[1]=type id, args[2]=sub id, args[3]=out. */
s32 Entity_QueryField(s32 *args[]) {
    s32 key;
    s32 sel;
    FieldActor *node;

    key = *args[1];
    if (key == 0) {
        FieldActor *tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto not_found;
        }
        node = tmp;
        goto found;
    }
    node = g_CurrentEntity;
    if ((key != node->type_id) || (*args[2] != node->sub_id)) {
        node = g_FieldActorListHead;
        if (node != NULL) {
            s32 k = *(volatile s32 *)*(s32 * volatile *)&args[1];
loop:
            if ((node->type_id != k) || (node->sub_id != *args[2]) || (node->flags & 0x10)) {
                node = node->next;
                if (node != NULL) {
                    goto loop;
                }
            }
            if (node == NULL) {
                goto not_found;
            }
            goto found;
        }
not_found:
        *args[3] = -1;
        return 1;
    }
found:
    sel = *args[0];
    if (sel == 1) {
        goto case1;
    }
    if (sel < 2) {
        if (sel == 0) {
            goto case0;
        }
        return 1;
    }
    if (sel == 2) {
        goto case2;
    }
    if (sel == 3) {
        goto case3;
    }
    return 1;
case0:
    *args[3] = node->mode;
    goto done;
case1:
    *args[3] = node->flags;
    goto done;
case2:
    *args[3] = (s16)node->anim_frame;
    goto done;
case3:
    *args[3] = node->action;
done:
    return 1;
}
