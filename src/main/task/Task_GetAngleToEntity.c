typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
#define NULL ((void *)0)
#include "pe1/field_actor.h"

int Gte_Atan2(int arg0, int arg1);

extern FieldActor *g_PlayerEntity[];
#define g_PlayerEntity (g_PlayerEntity[0])
extern FieldActor *g_FieldActorListHead[];
#define g_FieldActorListHead (g_FieldActorListHead[0])
extern FieldActor *g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])

/* Script op: angle from the current entity to the actor named by
 * (args[0], args[1]), relative to the current entity's yaw, into args[2]. */
s32 Task_GetAngleToEntity(s32 *args[]) {
    s32 key;
    s32 key2;
    FieldActor *node;
    s32 angle;
    s32 dx;
    s32 dz;

    key = *args[0];
    if (key == 0) {
        FieldActor *tmp;

        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto fail;
        }
        node = tmp;
        goto found;
    } else {
        key2 = key;
        node = g_FieldActorListHead;
        if (node == NULL) {
            goto fail;
        }
loop:
        if ((node->type_id != key2) ||
            (node->sub_id != *args[1]) ||
            (node->flags & 0x10)) {
            node = node->next;
            if (node != NULL) {
                goto loop;
            }
        }
        if (node != NULL) {
            goto found;
        }
    }

fail:
    *args[2] = -1;
    return 1;

found:
    {
        FieldActor *state = g_CurrentEntity;
        dx = state->pos_x - node->pos_x;
        dz = (state->pos_z - node->pos_z) >> 16;
    }
    angle = 0x1400 - Gte_Atan2(dz, dx >> 16);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= (s16)g_CurrentEntity->rot_y;
    if (angle < 0) {
        angle += 0x1000;
    }
    *args[2] = angle;
    return 1;
}
