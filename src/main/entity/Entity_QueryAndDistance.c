#include "common.h"

/* Adjacent field-actor query and distance routines share one compiled unit. */
#define NULL ((void *)0)
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"

extern FieldActor *g_FieldActorListHead;
extern FieldActor *g_CurrentEntity;

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
            *args[3] = -1;
            return 1;
        }
        node = tmp;
    } else {
        node = g_CurrentEntity;
        if ((key != node->type_id) || (*args[2] != node->sub_id)) {
            node = g_FieldActorListHead;
            if (node != NULL) {
                s32 k = *(s32 *)*(s32 * volatile *)&args[1];
                while (node != NULL && ((node->type_id != k) ||
                       (node->sub_id != *args[2]) || (node->flags & 0x10))) {
                    node = node->next;
                }
            }
            if (node == NULL) {
                *args[3] = -1;
                return 1;
            }
        }
    }
    sel = *args[0];
    switch (sel) {
    case 0:
        *args[3] = node->mode;
        break;
    case 1:
        *args[3] = node->flags;
        break;
    case 2:
        *args[3] = (s16)node->anim.parts.integer;
        break;
    case 3:
        *args[3] = node->action;
        break;
    default:
        return 1;
    }
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
    int sourceX;
    s32 key0;
    FieldActor *node;

    key0 = *args[0];
    if (key0 == 0) {
        FieldActor *tmp;
        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            *args[2] = -1;
            return 1;
        }
        node = tmp;
    } else {
        node = g_FieldActorListHead;
        if (node != NULL) {
            s32 key;
            key = key0;
            while (node != NULL && ((node->type_id != key) ||
                   (node->sub_id != *args[1]) || (node->flags & 0x10))) {
                node = node->next;
            }
        }
        if (node == NULL) {
            *args[2] = -1;
            return 1;
        }
    }

    {
        FieldActor *state;
        s32 node_x;
        s32 *out;
        state = g_CurrentEntityForDistanceX[0];
        sourceX = state->pos_x;
        node_x = node->pos_x;
        out = args[2];
        var_v0 = sourceX;
        var_v0 = var_v0 - node_x;
        if (var_v0 < 0) {
            var_v0 = node_x - sourceX;
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
