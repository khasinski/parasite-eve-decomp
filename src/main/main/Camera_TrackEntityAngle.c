#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))

extern u8 *D_8009D2F0[];
extern u8 *D_8009D254[];
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

    if ((u8 *)args != D_8009D254[0]) {
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
