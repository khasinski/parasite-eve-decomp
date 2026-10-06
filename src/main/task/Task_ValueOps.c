/* MASPSX_FLAGS: --expand-div */
/* Script value opcodes: expression evaluation, current-actor vector get/set
 * and pad/flag mask tests. Contiguous default-profile handlers between the
 * node pool and the node actions; --expand-div serves the expression
 * divide and remainder and leaves the others unchanged. */
#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_PlayerEntity;
extern int g_RenderStateFlags[];
extern u32 D_8009D26C[];
extern u32 D_8009D1F4[];
extern u32 D_8009D1E4[];
extern u32 D_800A7770[];

int Math_FixedMul(int lhs, int rhs);
int Math_FixedDivide(int a, int b);
void Entity_FindFloor(FieldActor *entity);

typedef struct ExprOpArgs {
    int *op;
    int *dst;
    int *lhs;
    int *rhs;
} ExprOpArgs;

int Task_EvalExpr(ExprOpArgs *args) {
    switch (*args->op) {
    case 0:
        *args->dst = *args->lhs + *args->rhs;
        break;
    case 1:
        *args->dst = *args->lhs - *args->rhs;
        break;
    case 2:
        *args->dst = *args->lhs | *args->rhs;
        break;
    case 3:
        *args->dst = *args->lhs & *args->rhs;
        break;
    case 4:
        *args->dst = *args->lhs ^ *args->rhs;
        break;
    case 5:
        *args->dst = (*args->lhs || *args->rhs);
        break;
    case 6:
        *args->dst = (*args->lhs && *args->rhs);
        break;
    case 7:
        *args->dst = !*args->lhs;
        break;
    case 8:
        *args->dst = ~*args->lhs;
        break;
    case 9:
        *args->dst = *args->lhs > *args->rhs;
        break;
    case 10:
        *args->dst = *args->lhs < *args->rhs;
        break;
    case 11:
        *args->dst = *args->lhs == *args->rhs;
        break;
    case 12:
        *args->dst = *args->lhs >= *args->rhs;
        break;
    case 13:
        *args->dst = *args->lhs <= *args->rhs;
        break;
    case 14:
        *args->dst = *args->lhs != *args->rhs;
        break;
    case 15:
        *args->dst = *args->lhs * *args->rhs;
        break;
    case 16:
        *args->dst = *args->lhs / *args->rhs;
        break;
    case 17:
        *args->dst = *args->lhs << *args->rhs;
        break;
    case 18:
        *args->dst = *args->lhs >> *args->rhs;
        break;
    case 19:
        *args->dst = *args->lhs;
        break;
    case 20:
        *args->dst = Math_FixedMul(*args->lhs, *args->rhs);
        break;
    case 21:
        *args->dst = Math_FixedDivide(*args->lhs, *args->rhs);
        break;
    case 22:
        *args->dst = *args->lhs % *args->rhs;
        break;
    case 23:
        *args->dst = -*args->lhs;
        break;
    }
    return 1;
}

int Task_SetEntityVec3(int **args) {
    int **argp = args;

    switch (*argp[0]) {
    case 0:
        {
            FieldActor *state = g_CurrentEntity;

            state->pos_x = *argp[1];
            state->pos_y = *argp[2];
            state->pos_z = *argp[3];
            Entity_FindFloor((u8 *)state);
        }
        g_CurrentEntity->base_x = g_CurrentEntity->pos_x;
        g_CurrentEntity->base_y = g_CurrentEntity->pos_y;
        g_CurrentEntity->base_z = g_CurrentEntity->pos_z;
        if (g_CurrentEntity == g_PlayerEntity) {
            g_RenderStateFlags[0] |= 0x80;
        }
        break;
    case 1:
        g_CurrentEntity->base_x = *argp[1];
        g_CurrentEntity->base_y = *argp[2];
        g_CurrentEntity->base_z = *argp[3];
        break;
    case 2:
        g_CurrentEntity->motion_x = *argp[1];
        g_CurrentEntity->motion_y = *argp[2];
        g_CurrentEntity->motion_z = *argp[3];
        break;
    case 3:
        g_CurrentEntity->accel_x = *argp[1];
        g_CurrentEntity->accel_y = *argp[2];
        g_CurrentEntity->accel_z = *argp[3];
        break;
    case 4:
        g_CurrentEntity->gravity_x = *argp[1];
        g_CurrentEntity->gravity_y = *argp[2];
        g_CurrentEntity->gravity_z = *argp[3];
        break;
    case 5:
        g_CurrentEntity->rot_x = *argp[1];
        g_CurrentEntity->rot_y = *argp[2];
        g_CurrentEntity->rot_z = *argp[3];
        break;
    case 6:
        g_CurrentEntity->delta_x = *argp[1];
        g_CurrentEntity->delta_y = *argp[2];
        g_CurrentEntity->delta_z = *argp[3];
        break;
    }
    return 1;
}

int Task_GetEntityVec3(int **args) {
    switch (*args[0]) {
    case 0:
        *args[1] = g_CurrentEntity->pos_x;
        *args[2] = g_CurrentEntity->pos_y;
        *args[3] = g_CurrentEntity->pos_z;
        break;
    case 1:
        *args[1] = g_CurrentEntity->base_x;
        *args[2] = g_CurrentEntity->base_y;
        *args[3] = g_CurrentEntity->base_z;
        break;
    case 2:
        *args[1] = g_CurrentEntity->motion_x;
        *args[2] = g_CurrentEntity->motion_y;
        *args[3] = g_CurrentEntity->motion_z;
        break;
    case 3:
        *args[1] = g_CurrentEntity->accel_x;
        *args[2] = g_CurrentEntity->accel_y;
        *args[3] = g_CurrentEntity->accel_z;
        break;
    case 4:
        *args[1] = g_CurrentEntity->gravity_x;
        *args[2] = g_CurrentEntity->gravity_y;
        *args[3] = g_CurrentEntity->gravity_z;
        break;
    case 5:
        *args[1] = g_CurrentEntity->rot_x;
        *args[2] = g_CurrentEntity->rot_y;
        *args[3] = g_CurrentEntity->rot_z;
        break;
    case 6:
        *args[1] = g_CurrentEntity->delta_x;
        *args[2] = g_CurrentEntity->delta_y;
        *args[3] = g_CurrentEntity->delta_z;
        break;
    }
    return 1;
}
int Task_DispatchCmd(int **args) {
    int mode;

    mode = *args[0];
    switch (mode) {
    case 0: {
        u32 mask;
        u32 state;
        state = D_8009D26C[0];
        mask = *args[1];
        if ((state & mask) == mask) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
        break;
    }
    case 1: {
        u32 mask;
        u32 state;
        state = D_8009D1F4[0];
        mask = *args[1];
        if ((state & mask) == mask) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
        break;
    }
    case 2: {
        u32 mask;
        u32 state;
        state = D_8009D1E4[0];
        mask = *args[1];
        if ((state & mask) == mask) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
        break;
    }
    case 3: {
        u32 mask;
        u32 state;
        int index;

        index = (int)args[1];
        state = D_8009D26C[0];
        mask = *(u32 *)index;
        state &= mask;
        if (state == mask) {
            gte_ldlzcs(mask);
            index = 31;
            if (state != 0x80000000) {
                gte_stlzcr(args[1]);
                index -= *args[1];
            }
            *args[2] = D_800A7770[index];
        } else {
            *args[2] = 0;
        }
        break;
    }
    }
    return 1;
}
