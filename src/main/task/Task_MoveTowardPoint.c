#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
