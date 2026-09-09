#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

typedef struct GlobalPointerSlot {
    u8 *value;
    u8 pad[8];
} GlobalPointerSlot;

extern u8 *D_8009D20C[];
extern GlobalPointerSlot D_8009D254;
extern u8 *D_8009D2F0[];
extern u8 *D_8009D300;
extern int *D_8009CE00;

int Math_FixedMul(int a, int b);
int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);

int Camera_TrackEntityAngle(int **args) {
    int **script_args;
    u8 *current;
    u8 *tracking_entity;
    u8 *camera_state;
    int *turn_arg;
    int base_speed;
    int current_x;
    int current_z;
    int target_z;
    int target_x;
    int target_angle;
    int turn_speed;
    int current_angle;
    int angle_difference;
    int squared_x;
    int squared_z;
    int remaining_distance;
    int movement_distance;
    volatile int stack_pad[2];

    script_args = args;
    args = (int **)D_8009D2F0[0];
    current_x = S32_AT(args, 0x28);
    current_z = S32_AT(args, 0x30);

    if ((u8 *)args != D_8009D254.value) {
        base_speed = S32_AT(args, 0x20);
    } else {
        base_speed = Math_FixedMul(0x50000, S32_AT(args, 0x20));
    }
    base_speed = Math_FixedMul(base_speed, U16_AT(D_8009D2F0[0], 0x26) << 4);

    if ((U16_AT(D_8009D300, 8) & 0x20) == 0) {
        target_x = *script_args[0];
        target_z = *script_args[1];
        if (current_x == target_x && current_z == target_z) {
            return 1;
        }
        turn_arg = script_args[2];
        tracking_entity = D_8009D2F0[0];
        turn_speed = *turn_arg;
        if (S16_AT(tracking_entity, 0x3A) < 0) {
            S16_AT(tracking_entity, 0x3A) = S16_AT(tracking_entity, 0x3A) + 0x1000;
        }

        S32_AT(D_8009D300, 0x14) = target_x;
        S32_AT(D_8009D300, 0x18) = target_z;
        S32_AT(D_8009D300, 0x1C) = turn_speed;
        U16_AT(D_8009D300, 8) |= 0x20;
    } else {
        target_x = S32_AT(D_8009D300, 0x14);
        target_z = S32_AT(D_8009D300, 0x18);
        turn_speed = S32_AT(D_8009D300, 0x1C);
    }

    target_angle = (u16)(0x1400 - Gte_Atan2(current_z - target_z, current_x - target_x));
    target_angle &= 0xFFF;
    S32_AT(D_8009D2F0[0], 0x68) = Math_FixedMul(-base_speed, rsin(target_angle) << 4);
    base_speed = Math_FixedMul(-base_speed, rcos(target_angle) << 4);

    current = D_8009D2F0[0];
    angle_difference = S16_AT(current, 0x3A);
    current_angle = angle_difference;
    S32_AT(current, 0x70) = base_speed;
    if (current_angle < target_angle) {
        angle_difference = target_angle - current_angle;
        if (angle_difference < 0x801) {
            if (turn_speed < angle_difference) {
                S16_AT(current, 0x3A) = current_angle + turn_speed;
            }
        } else if (turn_speed < angle_difference) {
            S16_AT(current, 0x3A) = current_angle - turn_speed;
        }
    } else {
        angle_difference = current_angle - target_angle;
        if (angle_difference < 0x801) {
            if (turn_speed < angle_difference) {
                S16_AT(current, 0x3A) = current_angle - turn_speed;
            }
        } else if (turn_speed < angle_difference) {
            S16_AT(current, 0x3A) = current_angle + turn_speed;
        }
    }

    current = D_8009D2F0[0];
    current_x = (target_x - current_x) >> 16;
    current_z = (target_z - current_z) >> 16;
    squared_x = current_x * current_x;
    squared_z = current_z * current_z;
    remaining_distance = squared_x + squared_z;
    angle_difference = 0x6A;
    current_x = S16_AT(current, angle_difference);
    current_z = S16_AT(current, 0x72);
    movement_distance = current_x * current_x + current_z * current_z;
    S16_AT(current, 0x3A) &= 0xFFF;
    if (movement_distance >= remaining_distance) {
        camera_state = D_8009D300;
        S32_AT(current, 0x28) = target_x;
        S32_AT(current, 0x30) = target_z;
        S32_AT(current, 0x68) = 0;
        S32_AT(current, 0x70) = 0;
        U16_AT(camera_state, 8) &= 0xFFDF;
        return 1;
    } else {
        D_8009CE00 -= 5;
        S32_AT(D_8009D300, 0x10) = 1;
        return 0;
    }
}

int Camera_SnapToEntity(int **args) {
    u8 *target;
    u8 *current;
    int speed;
    int target_angle;
    int next_angle;
    int angle;
    int difference;
    int wrap_difference;
    u32 *target_x_ptr;
    s32 delta_x;
    s32 delta_z;

    if ((U16_AT(D_8009D300, 8) & 0x20) == 0) {
        if (*args[0] == 0) {
            if (D_8009D254.value == 0) {
                return 1;
            }
            target = D_8009D254.value;
        } else {
            target = D_8009D20C[0];
            while (target != 0) {
                if (U8_AT(target, 0xC) == *args[0] &&
                    U8_AT(target, 0xD) == *args[1] &&
                    (U32_AT(target, 0x98) & 0x10) == 0) {
                    break;
                }
                target = PTR_AT(target, 4);
            }
            if (target == 0) {
                return 1;
            }
        }

        speed = *args[2];
        PTR_AT(D_8009D300, 0x18) = target;
        U16_AT(D_8009D300, 8) |= 0x20;
        *(int *)(D_8009D300 + 0x14) = speed;
    } else {
        target = PTR_AT(D_8009D300, 0x18);
        if ((U32_AT(target, 0x98) & 0x10) != 0) {
            U16_AT(D_8009D300, 8) &= 0xFFDF;
            return 1;
        }
        speed = *(int *)(D_8009D300 + 0x14);
    }

    target_x_ptr = (u32 *)(target + 0x28);
    delta_x = U32_AT(D_8009D2F0[0], 0x28) - *target_x_ptr;
    delta_z = U32_AT(D_8009D2F0[0], 0x30) - U32_AT(target, 0x30);
    target_angle = 0x1400 - Gte_Atan2(delta_z >> 16, delta_x >> 16);
    next_angle = (target_angle &= 0xFFF);
    current = D_8009D2F0[0];
    angle = S16_AT(current, 0x3A);

    if (target_angle != angle) {
        if (angle < target_angle) {
            difference = target_angle - angle;
            if (difference < 0x800) {
                if (speed < difference) {
                    next_angle = angle + speed;
                }
            } else if (speed < difference) {
                next_angle = angle - speed;
                if (next_angle < 0) {
                    wrap_difference = angle + 0x1000;
                    wrap_difference -= target_angle;
                    if (wrap_difference < speed) {
                        next_angle = target_angle;
                    }
                }
            }
        } else {
            difference = angle - target_angle;
            if (difference < 0x800) {
                if (speed < difference) {
                    next_angle = angle - speed;
                }
            } else if (speed < difference) {
                next_angle = angle + speed;
                if (next_angle >= 0x1001) {
                    wrap_difference = target_angle + 0x1000;
                    wrap_difference -= angle;
                    if (wrap_difference < speed) {
                        next_angle = target_angle;
                    }
                }
            }
        }

        next_angle &= 0xFFF;
        S16_AT(D_8009D2F0[0], 0x3A) = next_angle;
        if (next_angle != target_angle) {
            D_8009CE00 -= 5;
            *(int *)(D_8009D300 + 0x10) = 1;
            return 0;
        }
    }

    U16_AT(D_8009D300, 8) &= 0xFFDF;
    return 1;
}

int Camera_TrackEntityZoom(int **args) {
    u8 *speed_entity;
    u8 *motion_entity;
    u8 *target;
    u8 *camera_state;
    int target_move_id;
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

    if ((U16_AT(D_8009D300, 8) & 0x20) == 0) {
        if (*args[0] == 0) {
            if (D_8009D254.value == 0) {
                return 1;
            }
            target = D_8009D254.value;
        } else {
            target = D_8009D20C[0];
            while (target != 0) {
                if (U8_AT(target, 0xC) == *args[0] &&
                    U8_AT(target, 0xD) == *args[1] &&
                    (U32_AT(target, 0x98) & 0x10) == 0) {
                    break;
                }
                target = PTR_AT(target, 4);
            }
            if (target == 0) {
                return 1;
            }
        }

        U16_AT(D_8009D300, 8) |= 0x20;
        turn_speed = *args[2];
        PTR_AT(D_8009D300, 0x18) = target;
        S32_AT(D_8009D300, 0x14) = turn_speed;
        target_move_id = U16_AT(target, 0x24);
        S32_AT(D_8009D300, 0x1C) = target_move_id;
    } else {
        target = PTR_AT(D_8009D300, 0x18);
        turn_speed = S32_AT(D_8009D300, 0x14);
        if ((U32_AT(target, 0x98) & 0x10) != 0 ||
            U16_AT(target, 0x24) != S32_AT(D_8009D300, 0x1C)) {
            U16_AT(D_8009D300, 8) &= 0xFFDF;
            return 1;
        }
    }

    target_x = S32_AT(target, 0x28);
    target_z = S32_AT(target, 0x30);
    current_x = S32_AT(D_8009D2F0[0], 0x28);
    current_z = S32_AT(D_8009D2F0[0], 0x30);
    if (current_x == target_x && current_z == target_z) {
        return 1;
    }

    speed_entity = D_8009D2F0[0];
    if (speed_entity != D_8009D254.value) {
        movement_speed = S32_AT(speed_entity, 0x20);
    } else {
        movement_speed = Math_FixedMul(0x50000, S32_AT(speed_entity, 0x20));
    }
    movement_speed = Math_FixedMul(movement_speed, U16_AT(D_8009D2F0[0], 0x26) << 4);
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
