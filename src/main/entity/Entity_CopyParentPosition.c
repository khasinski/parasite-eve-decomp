/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_actor.h"

FieldActor *g_FieldActorListHead;

void Entity_CopyParentPosition(void) {
    FieldActor *cur = g_FieldActorListHead;

    while (cur != 0) {
        if (cur->parent != 0 && (cur->flags & 0x400000) != 0) {
            cur->pos_x = cur->parent->pos_x;
            cur->pos_y = cur->parent->pos_y;
            cur->pos_z = cur->parent->pos_z;
            cur->rot_x = cur->parent->rot_x;
            cur->rot_y = cur->parent->rot_y;
            cur->rot_z = cur->parent->rot_z;
        }
        cur = cur->next;
    }
}
