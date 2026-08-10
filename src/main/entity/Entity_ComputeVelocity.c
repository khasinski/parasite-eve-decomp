#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_PlayerEntity;

int Math_FixedMul(int arg0, int arg1);
int rsin(int arg0);
int rcos(int arg0);

int Entity_ComputeVelocity(void) {
    FieldActor *state = g_CurrentEntity;
    int temp;

    if (state == g_PlayerEntity) {
        temp = Math_FixedMul(0x50000, state->move_factor);
    } else {
        temp = state->move_factor;
    }

    temp = Math_FixedMul(temp, g_CurrentEntity->move_speed << 4);
    g_CurrentEntity->motion_x = Math_FixedMul(-temp, rsin(g_CurrentEntity->rot_y) << 4);
    g_CurrentEntity->motion_z = Math_FixedMul(-temp, rcos(g_CurrentEntity->rot_y) << 4);

    return 1;
}
