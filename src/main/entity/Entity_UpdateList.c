#include "pe1/player_entity.h"
#include "pe1/field_collision.h"

extern int g_FieldMoveLock;
extern FieldActor *g_FieldActorListHead;

void Entity_UpdateAndRender(FieldActor *arg0);

void Entity_UpdateList(void) {
    FieldActor *node;

    if (g_FieldMoveLock & 4) {
        Entity_ApplyCollisionResponse(g_PlayerEntity);
    }

    node = g_FieldActorListHead;
    while (node != 0) {
        if ((node->flags & 0x80) == 0) {
            Entity_UpdateAndRender(node);
        }
        node = node->next;
    }
}
