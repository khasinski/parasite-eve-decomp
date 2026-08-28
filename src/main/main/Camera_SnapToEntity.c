#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 *D_8009D20C[];
typedef struct GlobalPointerSlot {
    u8 *value;
    u8 pad[8];
} GlobalPointerSlot;

extern GlobalPointerSlot D_8009D254;
extern u8 *D_8009D2F0[];
extern u8 *D_8009D300;
extern int *D_8009CE00;

int Gte_Atan2(int y, int x);

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
