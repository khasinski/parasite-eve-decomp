typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[4]; } D_8009D2B0_o __asm__("D_8009D2B0");
extern struct { char _[12]; } D_800108E4_o __asm__("D_800108E4");

#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_8009D2B0 (*(s8 *)&D_8009D2B0_o)
#define D_800108E4 ((u8 *)&D_800108E4_o)

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(int *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))
#define DIV(lhs, rhs) ((lhs) / (rhs))

u64 Math_Int32ToDouble(int value);
u64 Math_Sqrt64(u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);
u64 Math_Mul64(u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);
u64 Math_Div64(u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);
int Math_DoubleToInt32(u32 lo, u32 hi);
int rand(void);
void Entity_SetActionMode(void *entity, int mode);
void Battle_UpdateEntityFacing(void *entity);

void Battle_StepEnemyMovement(void *entity) {
    u8 *target;
    u8 *extra;
    u8 *actor;
    u8 *action;
    u32 actor_flags;
    u32 target_flags;
    u32 action_flags;
    int step;
    int mode_scale;
    int turn_state;
    int direction;
    int base;
    int scaled;

    target = PTR_AT(entity, 0);
    extra = target + 0xCC;
    actor = D_8009D278;
    action = PTR_AT(actor, 0x68);
    actor_flags = U32_AT(actor, 0x4C);

    if (actor_flags & 0x80000) {
        u64 distance;
        u64 targets;
        u64 target_scale;
        u64 product;
        u64 root;
        u64 quotient;

        base = (int)(U16_AT(actor, 0x1E) - 0x19 + S16_AT(actor, 0x04)) * 6;
        distance = Math_Int32ToDouble(base);
        targets = Math_Int32ToDouble(D_8009D2B0 - 1);
        target_scale = Math_Sqrt64((u32)targets, (u32)(targets >> 32),
                                   0x9999999A, 0x3FB99999);
        product = Math_Mul64((u32)target_scale, (u32)(target_scale >> 32),
                             0, 0x3FF00000);
        root = Math_Sqrt64((u32)distance, (u32)(distance >> 32),
                           (u32)product, (u32)(product >> 32));
        quotient = Math_Div64((u32)root, (u32)(root >> 32), 0, 0x401C0000);
        step = Math_DoubleToInt32((u32)quotient, (u32)(quotient >> 32));
    } else if (actor_flags & 0x100000) {
        base = DIV(U16_AT(actor, 0x1E), 5) + S16_AT(action, 0);
        scaled = DIV(S16_AT(actor, 0x04) * 7 + 0x78, 100);
        base *= scaled;
        step = base + DIV(base * S16_AT(actor, 0x0A), S16_AT(actor, 0x2A));
    } else {
        base = DIV(U16_AT(actor, 0x1E), 5) + S16_AT(action, 0);
        step = DIV(base * D_800108E4[U32_AT(action, 0x10) & 0xF], 100);
    }

    if (!(actor_flags & 0x180000) && S16_AT(action, 0x06) != 8) {
        step -= DIV(U16_AT(target, 0x8C), U32_AT(action, 0x10) & 0xF);
    } else {
        step -= U16_AT(target, 0x8C);
    }

    if (step > 0) {
        if (!(actor_flags & 0x180000)) {
            target_flags = U32_AT(target, 0);
            mode_scale = 0;

            switch (target_flags & 0x38000) {
            case 0x08000:
                step = (step * 3) / 10;
                break;
            case 0x18000:
                step = (step * 3) / 2;
                break;
            case 0x20000:
                step /= 10;
                break;
            }

            turn_state = (U32_AT(target, 0) >> 18) & 3;
            if (turn_state == 3) {
                U32_AT(extra, 0) |= 0x1000000;
            } else if (turn_state != 2) {
                action_flags = U32_AT(action, 0x10);

                if (action_flags & 0x400) {
                    direction = U32_AT(extra, 0) & 3;
                    if ((direction == 0 && (rand() & 1) == 0) || direction == 2) {
                        U32_AT(target, 0) |= 0x400;
                    }
                }

                if (!(U32_AT(target, 0) & 0x400)) {
                    if (action_flags & 0x100) {
                        direction = (U32_AT(extra, 0) >> 14) & 3;
                        if (direction == 1) {
                            mode_scale = 1;
                        } else if (direction == 2) {
                            mode_scale = 2;
                        }
                    }

                    if (mode_scale != 2 && (action_flags & 0x200)) {
                        direction = U16_AT(extra, 2) & 3;
                        if (direction == 1) {
                            mode_scale = 1;
                        } else if (direction == 2) {
                            mode_scale = 2;
                        }
                    }

                    if (action_flags & 0x800) {
                        direction = (U32_AT(extra, 0) >> 2) & 3;
                        if ((direction == 0 && (rand() & 1) == 0) || direction == 2) {
                            U32_AT(target, 0) = (U32_AT(target, 0) | 0x10) & ~0x3E0;
                        }
                    }

                    if (action_flags & 0x1000) {
                        direction = (U32_AT(extra, 0) >> 4) & 3;
                        if ((direction == 0 && (rand() & 1) == 0) || direction == 2) {
                            U32_AT(target, 0) |= 0x1800;
                            U32_AT(entity, 0x98) |= 0x1000;
                            if (S8_AT(target, 5) != 0) {
                                Entity_SetActionMode(entity, (u16)S8_AT(target, 6));
                            }
                        }
                    }
                }
            }

            if (mode_scale == 1) {
                step /= 5;
            } else if (mode_scale == 2) {
                step = (step * 3) / 2;
            }
        }
    }

    if (step <= 0) {
        step = 1;
        if ((U32_AT(target, 0) & 0x38000) == 0x18000) {
            step = 2;
        }

        if ((U32_AT(target, 0) & 0xC0000) == 0xC0000) {
            U32_AT(extra, 0) |= 0x1000000;
        }
    }

    S32_AT(target, 0x10) -= step;

    if (!(U32_AT(D_8009D278, 0x4C) & 0x80000)) {
        Battle_UpdateEntityFacing(entity);
    }

    U32_AT(target, 0) = (U32_AT(target, 0) & ~0x6000) | 0x4000;
}
