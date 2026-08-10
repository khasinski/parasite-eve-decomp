#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Math_FixedDivide(int a, int b);
int Math_FixedMul(int arg0, int arg1);

int Entity_ComputeVelocityToTarget(int **arg0) {
    FieldActor *state;
    int delta28;
    int delta2C;
    int delta30;
    int scale;
    int temp;

    state = g_CurrentEntity;
    state->flags |= 2;

    delta28 = state->pos_x - *arg0[0];
    delta2C = state->pos_y - *arg0[1];
    delta30 = state->pos_z - *arg0[2];
    scale = *arg0[3] << 16;

    g_CurrentEntity->motion_x = Math_FixedDivide(delta28, scale);
    g_CurrentEntity->motion_z = Math_FixedDivide(delta30, scale);
    temp = Math_FixedMul(scale + 0x10000, g_CurrentEntity->gravity_y) >> 1;
    g_CurrentEntity->motion_y = temp;
    g_CurrentEntity->motion_y = Math_FixedDivide(delta2C, scale) + g_CurrentEntity->motion_y;

    return 1;
}
