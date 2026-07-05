/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;
typedef signed int s32;

extern char *g_ActiveActor[];
extern char *g_ActiveActor_late[] asm("g_ActiveActor");
extern char *g_ActiveActor_late2[] asm("g_ActiveActor");
extern char *g_PlayerEntity[];
extern void *g_BattlePendingEnemySpawn[];

int rand(void);
s16 Entity_ApplyHitAndSetAnim(void *arg0);
void Battle_ApplyEnemyAttack(u8 *ent);

#define U8(base, off) (*(u8 *)((char *)(base) + (off)))
#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))
#define S32(base, off) (*(s32 *)((char *)(base) + (off)))
#define PTR(base, off) (*(char **)((char *)(base) + (off)))

void Entity_ResolveDropTable(void *arg0) {
    char *state;
    char *entry;
    u16 scale;
    u32 flags;
    u32 word;
    int masked;
    int value;
    int roll;
    int tmp;

    state = g_ActiveActor[0];
    scale = U16(state, 0x20) / 5U;
    entry = *(char **)arg0;
    {
        register u32 flags_reg asm("$6");

        flags_reg = U32(state, 0x4C);
        flags = flags_reg;
    }

    if (flags & 0x1000) {
        scale = (u16)(((u16)scale * 3) / 10);
    }
    if (flags & 0x100) {
        scale = (u16)(((u16)scale * 3) / 10);
    }

    word = U32(entry, 0);
    if ((int)((word >> 21) & 7) < 3) {
        char *action;

        action = PTR(entry, 0x18);
        masked = U32(PTR(state, 0x6C), 0) & 0x3FF;
        if (U8(action, 0xE) == 0) {
            U8(action, 0) = 4;
        } else if (U8(action, 0xE) != 1) {
            U8(action, 0) = 3;
        }
    } else {
        masked = (U32(PTR(state, 0x6C), 0) >> 10) & 0x3FF;
    }

    value = U16(PTR(entry, 0x18), 0xC) - (u16)scale - masked;
    roll = rand();

    {
        char *chance_state;
        register int roll_mod asm("$2");
        int chance;

        roll_mod = roll % 100;
        chance_state = g_ActiveActor_late[0];
        chance = ((int)U8(entry, 0x90) *
                  (100 - (int)((U32(PTR(chance_state, 0x6C), 0) >> 20) & 0xFF))) /
                 100;
        if (roll_mod < chance) {
            tmp = value * 3;
            value = (s32)(tmp + ((u32)tmp >> 31)) >> 1;
            U32(chance_state, 0x4C) |= 0x8000;
        }
    }

    {
        char *after_state;

        after_state = g_ActiveActor_late2[0];
        if (!(U32(after_state, 0x4C) & 0x200)) {
            if (S32(after_state, 0x34) > 0) {
                value = (s32)(value + ((u32)value >> 31)) >> 1;
            }
            if (value > 0) {
                char *action;

                action = PTR(entry, 0x18);
                if (U8(action, 1) == 0xA) {
                    value = (s32)(value + ((u32)value >> 31)) >> 1;
                }
                U16(after_state, 0xC) = U16(after_state, 0xC) - value;
            }
            if (S16(g_ActiveActor[0], 0xC) != 0) {
                Entity_ApplyHitAndSetAnim(arg0);
                if (!(U32(g_PlayerEntity[0], 0x98) & 0x100)) {
                    g_BattlePendingEnemySpawn[0] = arg0;
                }
            }
        } else if (value > 0) {
            char *action;

            action = PTR(entry, 0x18);
            if (U8(action, 1) == 0xA) {
                value = (s32)(value + ((u32)value >> 31)) >> 1;
            }
            U32(after_state, 8) -= value * ((S32(after_state, 0x28) * 4) / S16(after_state, 0x1C));
        }
    }

    if (U8(PTR(entry, 0x18), 1) != 0) {
        Battle_ApplyEnemyAttack(entry);
    }
}
