/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;
typedef signed int s32;

extern char *g_PlayerEntity[];
extern char *g_PlayerEntity_call[] asm("g_PlayerEntity");
extern char *g_PlayerEntity_anim[] asm("g_PlayerEntity");
extern char *g_PlayerEntity_effect[] asm("g_PlayerEntity");
extern char *g_PlayerEntity_flags[] asm("g_PlayerEntity");
extern char *g_ActiveActor[];
extern u16 D_8009D298;
extern u8 D_8009D29A_abs[] asm("g_BattleHitActionMode");
extern u8 D_8009D29B_abs[] asm("g_BattleHitActionSubmode");
extern u32 D_8009D29C_abs[] asm("g_BattleHitActionParam");

s32 Battle_CalcRelativeAngle(void *arg0, void *actor);
void Entity_SetActionMode(void *actor, s32 bucket);
void Asset_Find08Alt(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

#define U8(base, off) (*(u8 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))

s16 Entity_ApplyHitAndSetAnim(void *arg0) {
    void *saved_arg;
    s32 value;
    char *switch_actor;
    char *actor;
    char *state;
    register s32 bucket asm("$2");
    u32 flags;

    saved_arg = arg0;
    value = 0;
    switch_actor = g_PlayerEntity[0];

    switch (U8(switch_actor, 0xE)) {
    case 6:
    case 8:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15: {
        char *case_actor;
        u8 mode;
        u8 submode;

        case_actor = g_PlayerEntity[0];
        mode = U8(case_actor, 0xE);
        D_8009D29A_abs[0] = mode;
        submode = U8(case_actor, 0xF);
        D_8009D298 = 1;
        D_8009D29B_abs[0] = submode;
        D_8009D29C_abs[0] = U32(case_actor, 0x14);
        break;
    }
    case 7:
    case 9:
    case 11: {
        char *case_actor;
        u8 mode;
        u8 submode;

        case_actor = g_PlayerEntity[0];
        mode = U8(case_actor, 0xE);
        D_8009D29A_abs[0] = mode;
        submode = U8(case_actor, 0xF);
        D_8009D298 = 1;
        D_8009D29B_abs[0] = submode;
        D_8009D29C_abs[0] = U8(case_actor, 0xF) << 16;
        break;
    }
    }

    state = g_ActiveActor[0];
    if ((U32(state, 0x4C) & 0x12000) == 0) {
        value = Battle_CalcRelativeAngle(saved_arg, g_PlayerEntity_call[0]);
        if ((s16)value < 0x200) {
            bucket = 0;
        } else if ((s16)value < 0x600) {
            bucket = 2;
        } else if ((s16)value < 0xA00) {
            bucket = 1;
        } else if ((s16)value < 0xE00) {
            bucket = 3;
        } else {
            bucket = 0;
        }

        Entity_SetActionMode(g_PlayerEntity_anim[0], bucket);

        {
            char *effect_actor;

            effect_actor = g_PlayerEntity_effect[0];
            Asset_Find08Alt(0x46A, 0, S16(effect_actor, 0x2A), S16(effect_actor, 0x2E), S16(effect_actor, 0x32));
        }

        {
            char *flag_actor;

            flag_actor = g_PlayerEntity_flags[0];
            flags = U32(flag_actor, 0x98);
            actor = flag_actor;
        }
        if (flags & 0x100) {
            U32(actor, 0x98) = flags & ~0x100;
            D_8009D298 = 2;
        }
    }

    return (s16)value;
}
