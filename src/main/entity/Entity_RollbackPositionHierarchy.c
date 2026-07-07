/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead;

void Entity_RollbackPositionHierarchy(FieldActor *arg0)
{
    FieldActor *cur;
    FieldActor *child;

    child = arg0->parent;
    if (child != 0) {
        Entity_RollbackPositionHierarchy(child);
        cur = g_FieldActorListHead;
        if (cur != 0) {
            do {
                if (cur->parent == arg0->parent) {
                    cur->pos_x = cur->base_x;
                    cur->pos_y = cur->base_y;
                    cur->pos_z = cur->base_z;
                    cur->field_1a4 = cur->field_1a8;
                }
                cur = cur->next;
            } while (cur != 0);
        }
    } else {
        arg0->pos_x = arg0->base_x;
        arg0->pos_y = arg0->base_y;
        arg0->pos_z = arg0->base_z;
        arg0->field_1a4 = arg0->field_1a8;
        arg0->flags |= 0x40000;
    }
}
