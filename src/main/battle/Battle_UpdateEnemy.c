typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern u32 D_8009D1A0;
extern u8 *D_8009D278;
extern u8 *D_8009D20C;
extern u8 *D_8009D254;

void Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);
void Battle_DrawStatusPanel(int arg0, void *arg1);
void Battle_ProcessActionSlot(void *slot);
void Battle_SlotFree(void *slot);
void Battle_StepEnemyMovement(void *slot);
void Battle_StepEntityAnimState(void *slot);
void Entity_SetActionMode(void *entity, int mode);
void Entity_TickAnimSequences(void *entity);
int rcos(int angle);
int rsin(int angle);

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

static void Battle_ClearVelocity(void *slot) {
    S32_AT(slot, 0x68) = 0;
    S32_AT(slot, 0x6C) = 0;
    S32_AT(slot, 0x70) = 0;
}

static void Battle_ResetEnemyAnimIfIdle(void *slot, u8 *enemy) {
    if ((U32_AT(enemy, 0) & 0x180E) == 0) {
        U32_AT(slot, 0x98) &= ~0x1000;
        Entity_TickAnimSequences(slot);
        if (PTR_AT(enemy, 0x18) != 0) {
            U8_AT(PTR_AT(enemy, 0x18), 0) = 4;
            U32_AT(enemy, 0) &= 0xC0FFFFFF;
        }
    }
}

static void Battle_StepEnemyFlagRing(void *slot, u8 *enemy, u32 mask, int shift, int count) {
    u32 flags;
    u32 value;

    flags = U32_AT(enemy, 0);
    if (flags & mask) {
        value = ((flags >> shift) & count) - 1;
        flags = (flags & ~mask) | ((value & count) << shift);
        U32_AT(enemy, 0) = flags;
        Battle_ResetEnemyAnimIfIdle(slot, enemy);
    }
}

static void Battle_UpdateEnemyCharge(void *slot, u8 *enemy) {
    int amount;
    int step;

    amount = U16_AT(enemy, 0x0C);
    if ((unsigned int)amount < 0x2328) {
        step = U16_AT(enemy, 0x8E);
        amount += step;
        U16_AT(enemy, 0x0C) = amount;
        if (U32_AT(enemy, 0) & 1) {
            amount -= (step * 2) / 5;
            U16_AT(enemy, 0x0C) = amount;
        }
    } else {
        U16_AT(enemy, 0x0C) = 0x2328;
        if (U32_AT(slot, 0x98) & 0x1000) {
            U16_AT(enemy, 0x0C) = 0;
        }
    }
}

static void Battle_UpdateEnemyCountdown(u8 *enemy) {
    int timer;

    if (U32_AT(enemy, 0) & 0x10) {
        timer = (U32_AT(enemy, 0) >> 5) & 0x1F;
        if (timer >= 0x1E) {
            S32_AT(enemy, 0x10) -= S16_AT(enemy, 0x96);
            U32_AT(enemy, 0) &= ~0x3E0;
        } else {
            U32_AT(enemy, 0) = (U32_AT(enemy, 0) & ~0x3E0) | (((timer + 1) & 0x1F) << 5);
        }
    }
}

static void Battle_SetActionAndMaybeRestorePose(void *slot, u8 *enemy) {
    int action;

    if (S8_AT(enemy, 5) == 0 && (U32_AT(enemy, 0) & 0xE) == 0 && U8_AT(slot, 0x0F) == U16_AT(slot, 0x1A)) {
        if (U8_AT(enemy, 0xBC) == 2 && (U32_AT(enemy, 0) & 0x1800) == 0) {
            U8_AT(enemy, 0xBC)--;
            Entity_SetActionMode(slot, U8_AT(enemy, 0xBD));
            if (U8_AT(enemy, 0xBE) != 0) {
                U32_AT(slot, 0x98) |= 0x200;
            }
            U32_AT(slot, 0x14) = U32_AT(enemy, 0xC0);
            U32_AT(slot, 0x18) = U32_AT(enemy, 0xC4);
            U32_AT(slot, 0x1C) = U32_AT(enemy, 0xC8);
        } else {
            action = S8_AT(enemy, 6);
            Entity_SetActionMode(slot, (u16)action);
        }

        U32_AT(enemy, 0) &= ~0x6000;
        if ((U32_AT(D_8009D278, 0x4C) & 0x80000) == 0 && (U32_AT(enemy, 0) & 0x1800) == 0) {
            U32_AT(slot, 0x98) &= ~0x1000;
            Entity_TickAnimSequences(slot);
        }
    } else {
        Battle_UpdateEnemySway(slot, enemy);
    }
}

static void Battle_UpdateEnemySway(void *slot, u8 *enemy) {
    int value;

    if (U8_AT(enemy, 0xA4) >= 2 && U8_AT(enemy, 0xA5) != 0) {
        value = U16_AT(enemy, 0xA6) * rsin(S16_AT(enemy, 0xA8));
        S32_AT(slot, 0x28) += value << 4;
        value = U16_AT(enemy, 0xA6) * rcos(S16_AT(enemy, 0xA8));
        S32_AT(slot, 0x30) += value << 4;
        U8_AT(enemy, 0xA5)--;
    }
}

static void Battle_MarkLinkedEntitiesDead(void *slot, u8 *enemy) {
    u8 *node;
    u8 *entity;

    if (S8_AT(enemy, 5) == 1) {
        U16_AT(PTR_AT(slot, 0x18C), 0x250) |= 0x20;
        return;
    }

    if (S8_AT(enemy, 5) == 4) {
        node = D_8009D20C;
        while (node != 0 && node != D_8009D254) {
            entity = PTR_AT(node, 0);
            if (entity != 0 && S8_AT(entity, 5) == 4) {
                U16_AT(node, 0x250) |= 0x20;
            }
            node = PTR_AT(node, 4);
        }
        return;
    }

    U16_AT(slot, 0x250) |= 0x20;
}

static int Battle_HasLivingLinkedEnemy(void) {
    u8 *node;
    u8 *entity;
    int type;

    node = D_8009D20C;
    while (node != 0) {
        entity = PTR_AT(node, 0);
        if (entity != 0 && node != D_8009D254) {
            type = S8_AT(entity, 5);
            if (type != 0 && type != 2 && type != 4 && S32_AT(entity, 0x10) > 0) {
                return 1;
            }
        }
        node = PTR_AT(node, 4);
    }
    return 0;
}

static void Battle_UpdateDeathCleanup(void *slot, u8 *enemy) {
    int type;

    if (S32_AT(enemy, 0x10) > 0 || (D_8009D1A0 & 0x100)) {
        return;
    }

    type = S8_AT(enemy, 5);
    if (type == 1 && (U32_AT(enemy, 0) & 0x6000) == 0) {
        U32_AT(slot, 0x98) |= 0x10;
        Battle_SlotFree(slot);
        type = S8_AT(enemy, 5);
    }

    if (type == 4) {
        if ((U32_AT(enemy, 0) & 0x6000) == 0) {
            Battle_ClearVelocity(slot);
            Battle_StepEntityAnimState(slot);
        }
    } else if (type != 1 && type != 3) {
        Battle_ClearVelocity(slot);
        Battle_StepEntityAnimState(slot);
    } else if (!Battle_HasLivingLinkedEnemy()) {
        if (S8_AT(enemy, 5) == 3) {
            Battle_ClearVelocity(slot);
            Battle_StepEntityAnimState(slot);
        } else if (S8_AT(enemy, 5) == 1) {
            U32_AT(slot, 0x98) |= 0x10;
            Battle_SlotFree(slot);
            Battle_ClearVelocity(PTR_AT(slot, 0x18C));
            Battle_StepEntityAnimState(PTR_AT(slot, 0x18C));
        }
    }
}

static void Battle_UpdateEnemyHpPopup(void *slot, u8 *enemy) {
    int delta;

    delta = S32_AT(enemy, 0x14) - S32_AT(enemy, 0x10);
    if (delta != 0) {
        if (delta < 0) {
            U16_AT(enemy, 0xD0) = -delta;
            U8_AT(enemy, 0xD7) = 1;
        } else {
            U16_AT(enemy, 0xD0) = delta;
            if ((U32_AT(enemy, 0) & 0x38000) == 0x18000) {
                U8_AT(enemy, 0xD7) = 2;
            } else {
                U8_AT(enemy, 0xD7) = 0;
            }
        }

        U16_AT(enemy, 0xD2) = U16_AT(slot, 0x218);
        U8_AT(enemy, 0xD6) = 0x1E;
        U16_AT(enemy, 0xD4) = U16_AT(slot, 0x21A) - 0x14;
    }

    if (U8_AT(enemy, 0xD6) != 0) {
        Battle_DrawStatusPanel(1, enemy + 0xD0);
        U8_AT(enemy, 0xD6)--;
    }
}

void Battle_UpdateEnemy(void *slot) {
    u8 *enemy;
    u8 *state;

    enemy = PTR_AT(slot, 0);
    state = enemy;

    if (S32_AT(enemy, 0x10) > S32_AT(enemy, 0x88)) {
        S32_AT(enemy, 0x10) = S32_AT(enemy, 0x88);
    }

    if ((D_8009D1A0 & 0x100) == 0) {
        if (U16_AT(enemy, 0x0C) == 0) {
            Battle_StepEnemyFlagRing(slot, enemy, 0xE, 1, 7);
            Battle_StepEnemyFlagRing(slot, state, 0x1800, 11, 3);
        }

        Battle_UpdateEnemyCharge(slot, enemy);
        Battle_UpdateEnemyCountdown(state);
    }

    if (U32_AT(state, 0) & 0x6000) {
        Battle_ProcessActionSlot(slot);
        if ((U32_AT(state, 0) & 0x6000) == 0x2000) {
            Battle_StepEnemyMovement(slot);
            Asset_Find08w(U16_AT(enemy, 0xB0), 0, S16_AT(slot, 0x268), S16_AT(slot, 0x26A), S16_AT(slot, 0x26C));
        } else if ((U32_AT(state, 0) & 0x6000) == 0x4000) {
            if (S8_AT(enemy, 5) == 0) {
                Battle_SetActionAndMaybeRestorePose(slot, enemy);
            } else if (U32_AT(state, 0) & 0xE) {
                U32_AT(state, 0) &= ~0x6000;
                if (S8_AT(enemy, 5) != 0 && S32_AT(enemy, 0x10) > 0) {
                    Battle_MarkLinkedEntitiesDead(slot, enemy);
                }
            }
        }

        if (S8_AT(enemy, 5) != 1) {
            Battle_ClearVelocity(slot);
        }
    }

    if ((U32_AT(state, 0) & 0xE) != 0 && S32_AT(enemy, 0x10) > 0) {
        Battle_ClearVelocity(slot);
        if ((D_8009D1A0 & 0x100) == 0) {
            S16_AT(slot, 0x3A) = (S16_AT(slot, 0x3A) + 0x80) & 0xFFF;
        }
    }

    if (U32_AT(state, 0) & 0x1800) {
        Battle_ClearVelocity(slot);
    }

    if (U32_AT(state, 0) & 0x400) {
        S32_AT(enemy, 0x10) = -1;
    }

    Battle_UpdateEnemyHpPopup(slot, enemy);
    Battle_UpdateDeathCleanup(slot, enemy);
    S32_AT(enemy, 0x14) = S32_AT(enemy, 0x10);
}
