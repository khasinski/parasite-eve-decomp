/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

#include "pe1/field_actor.h"

typedef unsigned char u8;

extern FieldActor *g_FieldActorListHead;

void Battle_SetContextField(int arg0, int arg1);
void Entity_WriteFieldByCmd(FieldActor *arg0, int arg1, int arg2);

int Entity_SetFieldByTypeId(int **arg0) {
    FieldActor *obj;
    register int type asm("$2");
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
