#include "pe1/field_actor.h"

extern FieldActor *g_PlayerEntity;
extern FieldActor *g_FieldActorListHead;
extern FieldActor *g_CurrentEntity;

int Entity_CopyAttachmentData(int **arg0) {
    register int **args asm("$6") = arg0;
    FieldActor *selected;
    RenderObjectEntity *src;

    asm volatile("" : "=r"(args) : "0"(args));

    if (*args[0] == 0) {
        register FieldActor *current asm("$5") = g_PlayerEntity;

        selected = current;
        if (current == 0) {
            return 1;
        }
        src = &selected->render_object;
    } else {
        int id = *args[0];

        selected = g_FieldActorListHead;
        if (selected == 0) {
            return 1;
        }

        while (selected != 0) {
            if (selected->type_id == id &&
                    selected->sub_id == *args[1] &&
                    ((selected->flags & 0x10) == 0)) {
                break;
            }
            selected = selected->next;
        }

        if (selected == 0) {
            return 1;
        }
        src = &selected->render_object;
    }

    Render_SetObjectAnim(&g_CurrentEntity->render_object, src, *(short *)args[2]);
    g_CurrentEntity->parent = selected;
    return 1;
}
