/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed short s16;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 *D_8009D20C;
extern u8 *D_8009D254;
extern u8 *D_8009D2F0;
extern u8 *D_8009D300;
extern int *D_8009CE00;

int Math_FixedMul(int a, int b);
int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);

int Camera_TrackEntityZoom(int **args) {
    u8 *current;
    u8 *target;
    int movement_speed;
    int turn_speed;
    int target_angle;
    int current_angle;
    int angle_difference;
    int dx;
    int dz;
    int remaining_distance;
    int movement_distance;

    if ((U16_AT(D_8009D300, 8) & 0x20) == 0) {
        if (*args[0] == 0) {
            target = D_8009D254;
        } else {
            target = D_8009D20C;
            while (target != 0) {
                if (U8_AT(target, 0xC) == *args[0] &&
                    U8_AT(target, 0xD) == *args[1] &&
                    (U32_AT(target, 0x98) & 0x10) == 0) {
                    break;
                }
                target = PTR_AT(target, 4);
            }
        }

        if (target == 0) {
            return 1;
        }

        turn_speed = *args[2];
        PTR_AT(D_8009D300, 0x18) = target;
        S32_AT(D_8009D300, 0x14) = turn_speed;
        U16_AT(D_8009D300, 8) |= 0x20;
        movement_speed = U16_AT(target, 0x24);
        S32_AT(D_8009D300, 0x1C) = movement_speed;
    } else {
        target = PTR_AT(D_8009D300, 0x18);
        movement_speed = S32_AT(D_8009D300, 0x14);
        if ((U32_AT(target, 0x98) & 0x10) != 0 ||
            U16_AT(target, 0x24) != S32_AT(D_8009D300, 0x1C)) {
            U16_AT(D_8009D300, 8) &= 0xFFDF;
            return 1;
        }
    }

    current = D_8009D2F0;
    if (S32_AT(current, 0x28) == S32_AT(target, 0x28) &&
        S32_AT(current, 0x30) == S32_AT(target, 0x30)) {
        return 1;
    }

    if (current == D_8009D254) {
        movement_speed = Math_FixedMul(0x50000, S32_AT(current, 0x20));
    } else {
        movement_speed = S32_AT(current, 0x20);
    }
    movement_speed = Math_FixedMul(movement_speed, U16_AT(current, 0x26) << 4);
    target_angle = (0x1400 - Gte_Atan2(S32_AT(current, 0x30) - S32_AT(target, 0x30),
                                        S32_AT(current, 0x28) - S32_AT(target, 0x28))) & 0xFFF;

    turn_speed = S32_AT(D_8009D300, 0x14);
    current_angle = S16_AT(current, 0x3A);
    if (turn_speed != 0) {
        if (current_angle < target_angle) {
            angle_difference = target_angle - current_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    current_angle = target_angle;
                } else {
                    current_angle += turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                current_angle = target_angle;
            } else {
                current_angle -= turn_speed;
                if (current_angle < 0 && current_angle + 0x1000 - target_angle < turn_speed) {
                    current_angle = target_angle;
                }
            }
        } else {
            angle_difference = current_angle - target_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    current_angle = target_angle;
                } else {
                    current_angle -= turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                current_angle = target_angle;
            } else {
                current_angle += turn_speed;
                if (current_angle >= 0x1001 && target_angle + 0x1000 - current_angle < turn_speed) {
                    current_angle = target_angle;
                }
            }
        }
        S16_AT(current, 0x3A) = current_angle & 0xFFF;
    }

    S32_AT(current, 0x68) = Math_FixedMul(-movement_speed, rsin(target_angle) << 4);
    S32_AT(current, 0x70) = Math_FixedMul(-movement_speed, rcos(target_angle) << 4);
    dx = (S32_AT(target, 0x28) - S32_AT(current, 0x28)) >> 16;
    dz = (S32_AT(target, 0x30) - S32_AT(current, 0x30)) >> 16;
    remaining_distance = dx * dx + dz * dz;
    movement_distance = S16_AT(current, 0x6A) * S16_AT(current, 0x6A) +
                        S16_AT(current, 0x72) * S16_AT(current, 0x72);
    if (movement_distance < remaining_distance) {
        D_8009CE00 -= 5;
        S32_AT(D_8009D300, 0x10) = 1;
        return 0;
    }

    S32_AT(current, 0x28) = S32_AT(target, 0x28);
    S32_AT(current, 0x30) = S32_AT(target, 0x30);
    S32_AT(current, 0x68) = 0;
    S32_AT(current, 0x70) = 0;
    U16_AT(D_8009D300, 8) &= 0xFFDF;
    return 1;
}
