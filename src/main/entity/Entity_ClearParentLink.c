#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_FieldActorListHead;

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
