#include "common.h"

/* Adjacent field-actor query and distance routines share one compiled unit. */
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
    *args[3] = (s16)node->anim.parts.integer;
    goto done;
case3:
    *args[3] = node->action;
done:
    return 1;
}

/* Same symbol; do not share the X-read address calculation with Y and Z. */
extern FieldActor *g_CurrentEntityForDistanceX[] asm("g_CurrentEntity");

s32 Entity_GetDistanceComponents(s32 *args[]) {
    s32 *temp_a0;
    s32 *temp_t0;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_a3;
    s32 temp_a3_2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 key0;
    FieldActor *node;

    key0 = *args[0];
    if (key0 == 0) {
        FieldActor *tmp;
        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto block_9;
        }
        node = tmp;
        goto block_10;
    }
    node = g_FieldActorListHead;
    if (node != NULL) {
        s32 key;
        key = key0;
loop_4:
        if ((node->type_id != key) || (node->sub_id != *args[1]) ||
            (node->flags & 0x10)) {
            node = node->next;
            if (node != NULL) {
                goto loop_4;
            }
        }
        if (node != NULL) {
            goto block_10;
        }
    }
    goto block_9;

block_9:
    *args[2] = -1;
    return 1;

block_10:
    {
        FieldActor *state;
        s32 node_x;
        register s32 state_x asm("$7");
        s32 *out;
        state = g_CurrentEntityForDistanceX[0];
        node_x = node->pos_x;
        state_x = state->pos_x;
        out = args[2];
        var_v0 = state_x - node_x;
        if (var_v0 < 0) {
            var_v0 = node_x - state_x;
        }
        *out = var_v0;
    }
    {
        FieldActor *state;
        s32 *out;
        state = g_CurrentEntity;
        out = args[2];
        temp_a3_2 = state->pos_y;
        temp_v0_2 = node->pos_y;
        temp_a2 = *out;
        temp_v1_2 = temp_a3_2 - temp_v0_2;
        if (temp_v1_2 >= 0) {
            var_v0_2 = temp_a2 + temp_v1_2;
        } else {
            var_v0_2 = temp_a2 + (temp_v0_2 - temp_a3_2);
        }
        *out = var_v0_2;
    }
    {
        FieldActor *state;
        s32 *out;
        state = g_CurrentEntity;
        out = args[2];
        temp_a2_2 = state->pos_z;
        temp_v0_3 = node->pos_z;
        temp_a1 = *out;
        temp_v1_3 = temp_a2_2 - temp_v0_3;
        if (temp_v1_3 >= 0) {
            var_v0_3 = temp_a1 + temp_v1_3;
        } else {
            var_v0_3 = temp_a1 + (temp_v0_3 - temp_a2_2);
        }
        *out = var_v0_3;
    }
    return 1;
}
