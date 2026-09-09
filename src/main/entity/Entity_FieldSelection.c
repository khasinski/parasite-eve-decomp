#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead;

void *Battle_GetContextField(int arg0);
void *Battle_GetEnemyContextField(FieldActor *arg0, int arg1);

int Entity_FindByTypeId(int **arg0) {
    FieldActor *obj;
    register int type;
    int cmpType;

    type = *arg0[0];
    if (type == 0) {
        *arg0[3] = (int)Battle_GetContextField(*(u8 *)arg0[2]);
    } else {
        obj = g_FieldActorListHead;
        cmpType = type;
        if (obj == 0) {
            return 1;
        }
loop:
        if (obj->type_id != cmpType) {
            goto next;
        }
        if (obj->sub_id != *arg0[1]) {
            goto next;
        }
        if ((obj->flags & 0x10) != 0) {
next:
            obj = obj->next;
            if (obj != 0) {
                goto loop;
            }
        }
        if (obj == 0) {
            return 1;
        }
        *arg0[3] = (int)Battle_GetEnemyContextField(obj, *(u8 *)arg0[2]);
    }
    return 1;
}

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

void Battle_SetContextField(int arg0, int arg1);
int Entity_SetField(char ***arg0) {
    FieldActor *ctx = g_CurrentEntity;

    if (ctx->type_id == 0) {
        Battle_SetContextField(*(unsigned char *)arg0[0], *(int *)arg0[1]);
    } else {
        Entity_WriteFieldByCmd(ctx, *(unsigned char *)arg0[0], *(int *)arg0[1]);
    }

    return 1;
}

#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead;

void Battle_SetContextField(int arg0, int arg1);
int Entity_SetFieldByTypeId(int **arg0) {
    FieldActor *obj;
    register int type;
    int cmpType;

    type = *arg0[0];
    if (type == 0) {
        Battle_SetContextField(*(u8 *)arg0[2], *arg0[3]);
    } else {
        obj = g_FieldActorListHead;
        cmpType = type;
        if (obj == 0) {
            return 1;
        }
loop:
        if (obj->type_id != cmpType) {
            goto next;
        }
        if (obj->sub_id != *arg0[1]) {
            goto next;
        }
        if ((obj->flags & 0x10) != 0) {
next:
            obj = obj->next;
            if (obj != 0) {
                goto loop;
            }
        }
        if (obj != 0) {
            Entity_WriteFieldByCmd(obj, *(u8 *)arg0[2], *arg0[3]);
        }
    }
    return 1;
}
