#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SceneDataTable0;
extern char *g_TaskNodePool;
extern char *g_CurrentEntity[];
/* Same symbol; keep the direction read separate from the later angle reads. */
extern char *g_CurrentEntityForDirection[] asm("g_CurrentEntity");

int Gte_Atan2(int arg0, int arg1);

int Task_TurnTowardPointStep(int **arg0) {
    char *node = g_TaskNodePool;
    int flags = *(u16 *)(node + 8);
    int x;
    int y;
    int step;
    register int angle asm("$3");
    register int stepped asm("$6");
    int original;

    {
        int active = flags & 0x20;
        if (active != 0) {
            goto cached_args;
        }
    }
    {
        int new_flags;

        x = *arg0[0];
        y = *arg0[1];
        step = *arg0[2];
        new_flags = flags | 0x20;
        *(u16 *)(node + 8) = new_flags;
        *(int *)(node + 0x14) = x;
        *(int *)(node + 0x1C) = step;
        *(int *)(node + 0x18) = y;
        goto have_args;
    }

cached_args:
    {
        x = *(int *)(node + 0x14);
        y = *(int *)(node + 0x18);
        step = *(int *)(node + 0x1C);
    }

have_args:
    {
        char *state;
        register int dx asm("$6");
        int dy;
        register int tmp asm("$2");

        state = g_CurrentEntityForDirection[0];
        tmp = *(int *)(state + 0x28);
        dx = (tmp - x) >> 16;
        dy = (*(int *)(state + 0x30) - y) >> 16;
        if ((dx | dy) == 0) {
            return 1;
        }
        angle = 0x1400 - Gte_Atan2(dy, dx);
    }

    {
        char *state;
        state = g_CurrentEntity[0];
        angle &= 0xFFF;
        original = *(short *)(state + 0x3A);
    }
    stepped = angle;
    if (angle == original) {
        goto finish;
    }

    {
        int current = original;
        register int desired asm("$3") = angle;
        register int out asm("$6") = stepped;
        int delta;

        if (current < desired) {
            delta = desired - current;
            if (delta < 0x800) {
                if (step < delta) {
                    out = current + step;
                }
            } else if (step < delta) {
                out = current - step;
                if (out < 0) {
                    int wrap = current + 0x1000;
                    wrap = wrap - desired;
                    if (wrap < step) {
                        out = desired;
                    }
                }
            }
        } else {
            delta = current - desired;
            if (delta < 0x800) {
                if (step < delta) {
                    out = current - step;
                }
            } else if (step < delta) {
                out = current + step;
                if (out >= 0x1001) {
                    int wrap = desired + 0x1000;
                    wrap = wrap - current;
                    if (wrap < step) {
                        out = desired;
                    }
                }
            }
        }
        stepped = out;
    }
    {
        char *state;
        state = g_CurrentEntity[0];
        stepped &= 0xFFF;
        *(u16 *)(state + 0x3A) = stepped;
    }
    if (stepped != angle) {
        int cursor = g_SceneDataTable0;
        char *active = g_TaskNodePool;
        cursor -= 0x14;
        g_SceneDataTable0 = cursor;
        *(int *)(active + 0x10) = 1;
        return 0;
    }

finish:
    {
        char *active = g_TaskNodePool;
        int f = *(u16 *)(active + 8);
        f &= 0xFFDF;
        *(u16 *)(active + 8) = f;
        return 1;
    }
}

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))

typedef struct GlobalPointerSlot {
    u8 *value;
    u8 pad[8];
} GlobalPointerSlot;

extern GlobalPointerSlot D_8009D254;
extern u8 *D_8009D2F0[];
extern u8 *D_8009D300;
extern int *D_8009CE00;

int Math_FixedMul(int a, int b);
int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);

int Task_MoveTowardPoint(int **args) {
    u8 *speed_entity;
    u8 *motion_entity;
    u8 *camera_state;
    int movement_speed;
    int turn_speed;
    int target_angle;
    int desired_angle;
    int current_angle;
    int angle_difference;
    int wrap_difference;
    int current_x;
    int current_z;
    int target_x;
    int target_z;
    int movement_z;
    int remaining_distance;
    int movement_distance;

    speed_entity = D_8009D2F0[0];
    current_x = S32_AT(speed_entity, 0x28);
    current_z = S32_AT(speed_entity, 0x30);
    if (speed_entity != D_8009D254.value) {
        movement_speed = S32_AT(speed_entity, 0x20);
    } else {
        movement_speed = Math_FixedMul(0x50000, S32_AT(speed_entity, 0x20));
    }
    movement_speed = Math_FixedMul(movement_speed, U16_AT(D_8009D2F0[0], 0x26) << 4);

    if ((U16_AT(D_8009D300, 8) & 0x20) == 0) {
        target_x = *args[0];
        target_z = *args[1];
        if (current_x == target_x && current_z == target_z) {
            return 1;
        }
        turn_speed = *args[2];
        S32_AT(D_8009D300, 0x14) = target_x;
        S32_AT(D_8009D300, 0x18) = target_z;
        S32_AT(D_8009D300, 0x1C) = turn_speed;
        U16_AT(D_8009D300, 8) |= 0x20;
    } else {
        camera_state = D_8009D300;
        target_x = S32_AT(camera_state, 0x14);
        target_z = S32_AT(camera_state, 0x18);
        turn_speed = S32_AT(camera_state, 0x1C);
    }

    target_angle = (u16)(0x1400 - Gte_Atan2(current_z - target_z,
                                             current_x - target_x));
    target_angle &= 0xFFF;
    desired_angle = target_angle;
    if (turn_speed != 0) {
        current_angle = S16_AT(D_8009D2F0[0], 0x3A);
        if (current_angle < desired_angle) {
            angle_difference = desired_angle - current_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    target_angle = desired_angle;
                } else {
                    target_angle = current_angle + turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                target_angle = desired_angle;
            } else {
                target_angle = current_angle - turn_speed;
                if (target_angle < 0) {
                    wrap_difference = current_angle + 0x1000;
                    wrap_difference -= desired_angle;
                    if (wrap_difference < turn_speed) {
                        target_angle = desired_angle;
                    }
                }
            }
        } else {
            angle_difference = current_angle - desired_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    target_angle = desired_angle;
                } else {
                    target_angle = current_angle - turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                target_angle = desired_angle;
            } else {
                target_angle = current_angle + turn_speed;
                if (target_angle >= 0x1001) {
                    wrap_difference = desired_angle + 0x1000;
                    wrap_difference -= current_angle;
                    if (wrap_difference < turn_speed) {
                        target_angle = desired_angle;
                    }
                }
            }
        }
        target_angle &= 0xFFF;
        S16_AT(D_8009D2F0[0], 0x3A) = target_angle;
    }

    S32_AT(D_8009D2F0[0], 0x68) = Math_FixedMul(-movement_speed, rsin(target_angle) << 4);
    movement_z = Math_FixedMul(-movement_speed, rcos(target_angle) << 4);
    current_x = (target_x - current_x) >> 16;
    current_z = (target_z - current_z) >> 16;
    remaining_distance = current_x * current_x + current_z * current_z;
    motion_entity = D_8009D2F0[0];
    current_x = S16_AT(motion_entity, 0x6A);
    current_z = movement_z >> 16;
    movement_distance = current_x * current_x + current_z * current_z;
    S32_AT(motion_entity, 0x70) = movement_z;
    if (movement_distance >= remaining_distance) {
        camera_state = D_8009D300;
        S32_AT(motion_entity, 0x28) = target_x;
        S32_AT(motion_entity, 0x30) = target_z;
        S32_AT(motion_entity, 0x68) = 0;
        S32_AT(motion_entity, 0x70) = 0;
        U16_AT(camera_state, 8) &= 0xFFDF;
        return 1;
    } else {
        D_8009CE00 -= 5;
        S32_AT(D_8009D300, 0x10) = 1;
        return 0;
    }
}
