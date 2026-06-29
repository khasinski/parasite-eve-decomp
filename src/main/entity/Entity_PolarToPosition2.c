/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int rsin(int arg0);
int rcos(int arg0);
int Math_FixedMul(int arg0, int arg1);

int Entity_PolarToPosition2(int **arg0) {
    int **args;
    int radius;
    FieldActor *current;
    FieldActor *current_v1;
    register int *src asm("$3");
    int *dst;
    int value;

    current = g_CurrentEntity;
    args = arg0;
    src = args[0];
    radius = *src;
    value = rsin((short)current->rot_y);
    radius = -radius;
    value = Math_FixedMul(radius, value << 4);
    current_v1 = g_CurrentEntity;
    dst = args[1];
    *dst = current_v1->pos_x + value;

    current = g_CurrentEntity;
    value = rcos((short)current->rot_y);
    value = Math_FixedMul(radius, value << 4);
    current_v1 = g_CurrentEntity;
    dst = args[2];
    *dst = current_v1->pos_z + value;
    return 1;
}
