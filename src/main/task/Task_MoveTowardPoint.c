#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))

extern u8 *D_8009D2F0;
extern u8 *D_8009D254;
extern u8 *D_8009D300;
extern int *D_8009CE00;

int Math_FixedMul(int a, int b);
int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);

int Task_MoveTowardPoint(int **args) {
    u8 *current;
    int base_speed;
    int target_x;
    int target_z;
    int turn_speed;
    int target_angle;
    int current_angle;
    int angle_difference;
    int dx;
    int dz;
    int remaining_distance;
    int movement_distance;

    current = D_8009D2F0;
    target_x = S32_AT(current, 0x28);
    target_z = S32_AT(current, 0x30);
    if (current == D_8009D254) {
        base_speed = Math_FixedMul(0x50000, S32_AT(current, 0x20));
    } else {
        base_speed = S32_AT(current, 0x20);
    }
    base_speed = Math_FixedMul(base_speed, U16_AT(current, 0x26) << 4);

    if ((U16_AT(D_8009D300, 8) & 0x20) == 0) {
        target_x = *args[0];
        target_z = *args[1];
        if (target_x == S32_AT(current, 0x28) && target_z == S32_AT(current, 0x30)) {
            return 1;
        }

        turn_speed = *args[2];
        S32_AT(D_8009D300, 0x14) = target_x;
        S32_AT(D_8009D300, 0x18) = target_z;
        S32_AT(D_8009D300, 0x1C) = turn_speed;
        U16_AT(D_8009D300, 8) |= 0x20;
    } else {
        target_x = S32_AT(D_8009D300, 0x14);
        target_z = S32_AT(D_8009D300, 0x18);
        turn_speed = S32_AT(D_8009D300, 0x1C);
    }

    target_angle = (0x1400 - Gte_Atan2(S32_AT(current, 0x30) - target_z,
                                        S32_AT(current, 0x28) - target_x)) & 0xFFF;
    if (turn_speed != 0) {
        current_angle = S16_AT(current, 0x3A);
        if (current_angle < target_angle) {
            angle_difference = target_angle - current_angle;
            if (angle_difference < 0x800) {
                if (turn_speed < angle_difference) {
                    current_angle += turn_speed;
                }
            } else if (turn_speed < angle_difference) {
                current_angle -= turn_speed;
                if (current_angle < 0 && current_angle + 0x1000 - target_angle < turn_speed) {
                    current_angle = target_angle;
                }
            }
        } else {
            angle_difference = current_angle - target_angle;
            if (angle_difference < 0x800) {
                if (turn_speed < angle_difference) {
                    current_angle -= turn_speed;
                }
            } else if (turn_speed < angle_difference) {
                current_angle += turn_speed;
                if (current_angle >= 0x1001 && target_angle + 0x1000 - current_angle < turn_speed) {
                    current_angle = target_angle;
                }
            }
        }
        S16_AT(current, 0x3A) = current_angle & 0xFFF;
    }

    S32_AT(current, 0x68) = Math_FixedMul(-base_speed, rsin(target_angle) << 4);
    S32_AT(current, 0x70) = Math_FixedMul(-base_speed, rcos(target_angle) << 4);
    dx = (target_x - S32_AT(current, 0x28)) >> 16;
    dz = (target_z - S32_AT(current, 0x30)) >> 16;
    remaining_distance = dx * dx + dz * dz;
    movement_distance = S16_AT(current, 0x6A) * S16_AT(current, 0x6A) +
                        S16_AT(current, 0x72) * S16_AT(current, 0x72);
    if (movement_distance < remaining_distance) {
        D_8009CE00 -= 5;
        S32_AT(D_8009D300, 0x10) = 1;
        return 0;
    }

    S32_AT(current, 0x28) = target_x;
    S32_AT(current, 0x30) = target_z;
    S32_AT(current, 0x68) = 0;
    S32_AT(current, 0x70) = 0;
    U16_AT(D_8009D300, 8) &= 0xFFDF;
    return 1;
}
