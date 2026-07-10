/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

#include "pe1/battle.h"

extern int g_BattleEntitySlotInUse[][0x37];
u8 D_8009D2A0;
u8 g_EntityInstanceCounter;

extern Combatant *D_8009D278;
extern int D_8009D1D0;
extern u8 D_8009D234[];
extern u8 D_8009D244;
extern u32 D_8009D2E8;
extern s16 D_8009D298[];
extern u8 D_8009D29A[];
extern u8 D_8009D29B[];
extern u32 D_8009D29C[];
extern u8 D_8009CE74;
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
extern s16 D_8009D2A4;
extern u32 D_8009D304;
extern u16 D_8009D21C;
extern struct { char _[16]; } D_800A7FF0_o __asm__("D_800A7FF0");
extern u32 D_8009D28C;
extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
extern int D_8009D1E8;
extern struct { char _[16]; } D_8009D290_o __asm__("D_8009D290");
extern u8 D_8009CE7C;
extern u8 D_8009CE78;
extern u8 D_8009D288;
extern struct { char _[16]; } D_8009D1AC_o __asm__("D_8009D1AC");
extern struct { char _[16]; } D_8009D1A8_o __asm__("D_8009D1A8");
extern struct { char _[16]; } D_8009D1CE_o __asm__("D_8009D1CE");
extern struct { char _[16]; } D_8009D235_o __asm__("D_8009D235");
extern struct { char _[16]; } D_800B8A90_o __asm__("D_800B8A90");
extern struct { char _[16]; } D_8009D250_o __asm__("D_8009D250");
extern struct { char _[16]; } D_8009D27C_o __asm__("D_8009D27C");
#define D_8009D254 (*(void **)&D_8009D254_o)
#define D_8009D1A0 (*(u32 *)&D_8009D1A0_o)
#define D_800A7FF0_addr ((void *)&D_800A7FF0_o)
#define D_800B0CD8 (*(u32 *)&D_800B0CD8_o)
#define D_8009D290 (*(u32 *)&D_8009D290_o)
#define D_8009D1AC (*(u32 *)&D_8009D1AC_o)
#define D_8009D1A8 (*(u32 *)&D_8009D1A8_o)
#define D_8009D1CE (*(u8 *)&D_8009D1CE_o)
#define D_8009D235 (*(u8 *)&D_8009D235_o)
#define D_800B8A90 ((u32 *)&D_800B8A90_o)
#define D_8009D250 (*(u32 *)&D_8009D250_o)
#define D_8009D27C (*(s16 *)&D_8009D27C_o)
#define D_8009D1AC_BYTE (*(u8 *)&D_8009D1AC_o)

void Battle_SaveAyaState(void);
void Save_ResetGlobalFlags(void);
void Battle_FlushScriptSounds(void);
void Tbl_ResetAll(void);
void Pm_StopAllBoth(void);
void Aya_SetTotalExp(u32 arg0, u16 arg1, void *arg2);
void Render_BeginSceneLoad(void);
void Entity_SetActionMode(void *entity, int mode);
int CD_StepReadState(int arg0);
void Battle_SetupPlayerPalette(void);
void srand(int seed);
void Battle_SetupEnemyAnims(void);
void Battle_CheckDropChance(void);
void Window_SetBoundsByMode(int mode);
int Entity_CheckActionIdMatch(void);
void BattleCmd_SyncActiveAmmo(void);

extern u8 D_8009D2EC;
extern struct { char _[16]; } D_800915E0_o __asm__("D_800915E0");
#define D_800915E0 (*(u32 *)&D_800915E0_o)
extern struct { char _[16]; } D_800B0CE6_palette_o __asm__("D_800B0CE6");
#define D_800B0CE6_PALETTE (*(u8 *)&D_800B0CE6_palette_o)

extern struct { char _[16]; } D_800B0134_o __asm__("D_800B0134");
#define D_800B0134 (*(s8 *)&D_800B0134_o)
extern struct { char _[16]; } D_800B0135_o __asm__("D_800B0135");
#define D_800B0135 (*(s8 *)&D_800B0135_o)
extern struct { char _[16]; } D_800B0136_o __asm__("D_800B0136");
#define D_800B0136 (*(s8 *)&D_800B0136_o)
extern struct { char _[16]; } D_800B013C_o __asm__("D_800B013C");
#define D_800B013C (*(s8 *)&D_800B013C_o)
extern struct { char _[16]; } D_800B013D_o __asm__("D_800B013D");
#define D_800B013D (*(s8 *)&D_800B013D_o)
extern struct { char _[16]; } D_800B013E_o __asm__("D_800B013E");
#define D_800B013E (*(s8 *)&D_800B013E_o)
extern struct { char _[16]; } D_800B0144_o __asm__("D_800B0144");
#define D_800B0144 (*(s8 *)&D_800B0144_o)
extern struct { char _[16]; } D_800B0145_o __asm__("D_800B0145");
#define D_800B0145 (*(s8 *)&D_800B0145_o)
extern struct { char _[16]; } D_800B0146_o __asm__("D_800B0146");
#define D_800B0146 (*(s8 *)&D_800B0146_o)
extern struct { char _[16]; } D_800B014C_o __asm__("D_800B014C");
#define D_800B014C (*(s8 *)&D_800B014C_o)
extern struct { char _[16]; } D_800B014D_o __asm__("D_800B014D");
#define D_800B014D (*(s8 *)&D_800B014D_o)
extern struct { char _[16]; } D_800B014E_o __asm__("D_800B014E");
#define D_800B014E (*(s8 *)&D_800B014E_o)
extern struct { char _[16]; } D_800B0158_o __asm__("D_800B0158");
#define D_800B0158 (*(s8 *)&D_800B0158_o)
extern struct { char _[16]; } D_800B0159_o __asm__("D_800B0159");
#define D_800B0159 (*(s8 *)&D_800B0159_o)
extern struct { char _[16]; } D_800B015A_o __asm__("D_800B015A");
#define D_800B015A (*(s8 *)&D_800B015A_o)
extern struct { char _[16]; } D_800B0160_o __asm__("D_800B0160");
#define D_800B0160 (*(s8 *)&D_800B0160_o)
extern struct { char _[16]; } D_800B0161_o __asm__("D_800B0161");
#define D_800B0161 (*(s8 *)&D_800B0161_o)
extern struct { char _[16]; } D_800B0162_o __asm__("D_800B0162");
#define D_800B0162 (*(s8 *)&D_800B0162_o)
extern struct { char _[16]; } D_800B0168_o __asm__("D_800B0168");
#define D_800B0168 (*(s8 *)&D_800B0168_o)
extern struct { char _[16]; } D_800B0169_o __asm__("D_800B0169");
#define D_800B0169 (*(s8 *)&D_800B0169_o)
extern struct { char _[16]; } D_800B016A_o __asm__("D_800B016A");
#define D_800B016A (*(s8 *)&D_800B016A_o)
extern struct { char _[16]; } D_800B0170_o __asm__("D_800B0170");
#define D_800B0170 (*(s8 *)&D_800B0170_o)
extern struct { char _[16]; } D_800B0171_o __asm__("D_800B0171");
#define D_800B0171 (*(s8 *)&D_800B0171_o)
extern struct { char _[16]; } D_800B0172_o __asm__("D_800B0172");
#define D_800B0172 (*(s8 *)&D_800B0172_o)
extern struct { char _[16]; } D_800B017C_o __asm__("D_800B017C");
#define D_800B017C (*(s8 *)&D_800B017C_o)
extern struct { char _[16]; } D_800B017D_o __asm__("D_800B017D");
#define D_800B017D (*(s8 *)&D_800B017D_o)
extern struct { char _[16]; } D_800B017E_o __asm__("D_800B017E");
#define D_800B017E (*(s8 *)&D_800B017E_o)
extern struct { char _[16]; } D_800B0184_o __asm__("D_800B0184");
#define D_800B0184 (*(s8 *)&D_800B0184_o)
extern struct { char _[16]; } D_800B0185_o __asm__("D_800B0185");
#define D_800B0185 (*(s8 *)&D_800B0185_o)
extern struct { char _[16]; } D_800B0186_o __asm__("D_800B0186");
#define D_800B0186 (*(s8 *)&D_800B0186_o)
extern struct { char _[16]; } D_800B018C_o __asm__("D_800B018C");
#define D_800B018C (*(s8 *)&D_800B018C_o)
extern struct { char _[16]; } D_800B018D_o __asm__("D_800B018D");
#define D_800B018D (*(s8 *)&D_800B018D_o)
extern struct { char _[16]; } D_800B018E_o __asm__("D_800B018E");
#define D_800B018E (*(s8 *)&D_800B018E_o)
extern struct { char _[16]; } D_800B0194_o __asm__("D_800B0194");
#define D_800B0194 (*(s8 *)&D_800B0194_o)
extern struct { char _[16]; } D_800B0195_o __asm__("D_800B0195");
#define D_800B0195 (*(s8 *)&D_800B0195_o)
extern struct { char _[16]; } D_800B0196_o __asm__("D_800B0196");
#define D_800B0196 (*(s8 *)&D_800B0196_o)
extern struct { char _[16]; } D_800B01A0_o __asm__("D_800B01A0");
#define D_800B01A0 (*(s8 *)&D_800B01A0_o)
extern struct { char _[16]; } D_800B01A1_o __asm__("D_800B01A1");
#define D_800B01A1 (*(s8 *)&D_800B01A1_o)
extern struct { char _[16]; } D_800B01A2_o __asm__("D_800B01A2");
#define D_800B01A2 (*(s8 *)&D_800B01A2_o)
extern struct { char _[16]; } D_800B01A8_o __asm__("D_800B01A8");
#define D_800B01A8 (*(s8 *)&D_800B01A8_o)
extern struct { char _[16]; } D_800B01A9_o __asm__("D_800B01A9");
#define D_800B01A9 (*(s8 *)&D_800B01A9_o)
extern struct { char _[16]; } D_800B01AA_o __asm__("D_800B01AA");
#define D_800B01AA (*(s8 *)&D_800B01AA_o)
extern struct { char _[16]; } D_800B01B0_o __asm__("D_800B01B0");
#define D_800B01B0 (*(s8 *)&D_800B01B0_o)
extern struct { char _[16]; } D_800B01B1_o __asm__("D_800B01B1");
#define D_800B01B1 (*(s8 *)&D_800B01B1_o)
extern struct { char _[16]; } D_800B01B2_o __asm__("D_800B01B2");
#define D_800B01B2 (*(s8 *)&D_800B01B2_o)
extern struct { char _[16]; } D_800B01B8_o __asm__("D_800B01B8");
#define D_800B01B8 (*(s8 *)&D_800B01B8_o)
extern struct { char _[16]; } D_800B01B9_o __asm__("D_800B01B9");
#define D_800B01B9 (*(s8 *)&D_800B01B9_o)
extern struct { char _[16]; } D_800B01BA_o __asm__("D_800B01BA");
#define D_800B01BA (*(s8 *)&D_800B01BA_o)

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_func_8002B29C(void);

void Save_InitSystem(void) {
    u8 i;

    Battle_SaveAyaState();

    for (i = 0; i < 7; i++) {
        g_BattleEntitySlotInUse[i][0] = 0;
    }

    D_8009D2A0 = 0;
    g_EntityInstanceCounter = 0;
    Save_ResetGlobalFlags();
}

void Battle_ResetEnemyStats(int mode) {
    register Combatant *actor asm("$5");
    register Combatant *clear_actor asm("$4");
    register s32 flags asm("$2");

    actor = D_8009D278;
    if ((s16)actor->maxHP < (s16)actor->curHP) {
        actor->curHP = actor->maxHP;
    }

    D_8009D278->actionMode12 = 4;
    actor = D_8009D278;
    actor->hpAlive = 0;
    actor->atbGauge = 0;
    actor->hpMirror = actor->curHP;
    D_8009D1D0 = 0;

    if ((mode & 0xFF) == 1) {
        D_8009D234[0] = 0x5A;
        D_8009D244 = 1;
        asm volatile("" ::: "memory");
        flags = actor->stateFlags;
        flags |= 0x800000;
    } else {
        flags = actor->stateFlags;
        D_8009D244 = 0;
        flags &= 0xFFBFFFFF;
        flags &= 0xFF7FFFFF;
    }
    asm volatile("sw %0,0x4C(%1)" : : "r"(flags), "r"(actor));

    clear_actor = D_8009D278;
    flags = clear_actor->stateFlags;
    clear_actor->panelA_timer = 0;
    flags &= -4;
    flags &= -0xD;
    flags &= -0x31;
    flags &= -0xC1;
    flags &= -0x101;
    flags &= -0x201;
    flags &= -0x401;
    flags &= -0x801;
    flags &= -0x1001;
    flags &= -0x2001;
    flags &= -0x4001;
    flags &= 0xFFFF7FFF;
    flags &= 0xFFF7FFFF;
    flags &= 0xFFFEFFFF;
    flags &= 0xFFF9FFFF;
    flags &= 0xFFEFFFFF;
    flags &= 0xFFDFFFFF;
    flags &= 0xFEFFFFFF;
    flags &= 0xF1FFFFFF;
    flags &= 0xEFFFFFFF;
    flags &= 0xDFFFFFFF;
    clear_actor->stateFlags = flags;

    D_8009D278->panelB_timer = 0;
    D_8009D278->field66 = 0;
    asm volatile(
        ".set noat\n"
        "lui $2, %%hi(D_8009D2E8)\n"
        "lw $2, %%lo(D_8009D2E8)($2)\n"
        "addiu $3, $0, -0x11\n"
        "and $2, $2, $3\n"
        "lui $1, %%hi(D_8009D2E8)\n"
        "sw $2, %%lo(D_8009D2E8)($1)\n"
        ".set at"
        :
        :
        : "$1", "$2", "$3", "memory");
    Battle_FlushScriptSounds();
    Tbl_ResetAll();
    D_8009D298[0] = 0;
    D_8009D29A[0] = 0;
    D_8009D29B[0] = 0;
    D_8009D29C[0] = 0;
}

void Battle_SetupPlayerPalette(void) {
    register int c2 asm("$2");
    register int c3 asm("$3");
    register int c4 asm("$4");
    register int c5 asm("$5");
    register int c6 asm("$6");
    register int c7 asm("$7");
    u8 *flags;

    c6 = 0x82;
    c5 = 0x36;
    c4 = 0x4A;
    c2 = 0xFF;
    c3 = 0x3B;
    c7 = 0x3D;

    D_800B0135 = c6;
    D_800B0145 = c6;
    D_800B017D = c6;
    D_800B018D = c6;

    c6 = 0x81;
    D_800B0136 = c5;
    D_800B0146 = c5;
    D_800B017E = c5;
    D_800B018E = c5;

    c5 = 0x83;
    D_800B013C = c4;
    D_800B014C = c4;
    D_800B0184 = c4;
    D_800B0194 = c4;

    c4 = 0x13;
    D_800B013E = c3;
    D_800B014E = c3;
    D_800B0186 = c3;
    D_800B0196 = c3;

    c3 = 1;
    D_800B0134 = 0;
    D_800B013D = c2;
    D_800B0144 = 0;
    D_800B014D = c2;
    D_800B017C = 0;
    D_800B0185 = c2;
    D_800B018C = 0;
    D_800B0195 = c2;

    D_800B0158 = c2;
    D_800B0159 = c7;
    D_800B015A = c6;
    D_800B0160 = c5;
    D_800B0161 = c4;
    D_800B0162 = c3;
    D_800B0168 = c2;
    D_800B0169 = c7;
    D_800B016A = c6;
    D_800B0170 = c5;
    D_800B0171 = c4;
    D_800B0172 = c3;
    D_800B01A0 = c2;
    D_800B01A1 = c7;
    D_800B01A2 = c6;
    D_800B01A8 = c5;
    D_800B01A9 = c4;
    D_800B01AA = c3;
    D_800B01B0 = c2;
    D_800B01B1 = c7;
    D_800B01B2 = c6;
    D_800B01B8 = c5;
    D_800B01B9 = c4;
    D_800B01BA = c3;

    Battle_FlushScriptSounds();
    D_8009D2EC = 0;
    D_8009D2A0 = 0;
    *(u32 *)((u8 *)D_8009D254 + 0x194) = D_800915E0;
    D_8009D1A0 &= -3;
    D_8009D2E8 &= -0x11;
    flags = &D_800B0CE6_PALETTE;
    *flags |= 2;
    BattleCmd_SyncActiveAmmo();
}

void Battle_StartEncounter(int mode) {
    register int i asm("$4");
    register int neg_one asm("$5");
    register void *entity asm("$2");
    register void *actor asm("$3");
    u32 value;
    register int mode_reg asm("$16");

    entity = D_8009D254;
    D_8009D278 = *(Combatant **)entity;
    D_8009D1E8 = 0;
    D_8009D290 = 0;
    D_8009D28C = 0;
    D_8009CE7C = 0;
    D_8009CE78 = 0;
    D_8009D288 = 0;
    D_8009CE74 = 0;
    mode_reg = mode;
    Save_ResetGlobalFlags();

    i = 0;
    neg_one = -1;
    asm volatile(
        ".set noat\n"
        "lui $1, %%hi(D_8009D1AC)\n"
        "sb $0, %%lo(D_8009D1AC)($1)\n"
        "lui $3, %%hi(D_8009D1AC)\n"
        "lw $3, %%lo(D_8009D1AC)($3)\n"
        "addiu $2, $0, -0x301\n"
        "lui $1, %%hi(D_8009D1A8)\n"
        "sw $0, %%lo(D_8009D1A8)($1)\n"
        "lui $1, %%hi(D_8009D1CE)\n"
        "sb $0, %%lo(D_8009D1CE)($1)\n"
        "lui $1, %%hi(D_8009D235)\n"
        "sb $0, %%lo(D_8009D235)($1)\n"
        "sw $0, %%gp_rel(D_8009D304)($gp)\n"
        "sh $0, %%gp_rel(D_8009D21C)($gp)\n"
        "and $3, $3, $2\n"
        "lui $1, %%hi(D_8009D1AC)\n"
        "sw $3, %%lo(D_8009D1AC)($1)\n"
        ".set at"
        :
        : "r"(i), "r"(neg_one)
        : "$1", "$2", "$3", "memory");

    for (; (u8)i < 0xA; i++) {
        *(u16 *)((u8 *)D_800A7FF0_addr + ((u8)i << 2)) = 0;
        *(u16 *)((u8 *)D_800A7FF0_addr + ((u8)i << 2) + 2) = neg_one;
    }

    for (i = 0; (u8)i < 7; i++) {
        D_800B8A90[(u8)i] = 0;
    }

    srand(D_8009D250);
    Battle_ResetEnemyStats(0);
    Battle_SetupEnemyAnims();

    actor = D_8009D278;
    value = *(u32 *)((u8 *)actor + 8);
    if ((s32)value <= 0) {
        *(u32 *)((u8 *)actor + 8) = 0x10000;
    } else {
        u32 max_value;

        max_value = *(u32 *)((u8 *)actor + 0x28);
        if ((s32)value >= (s32)max_value) {
            *(u32 *)((u8 *)actor + 8) = max_value;
            *(u32 *)((u8 *)actor + 0x34) = 0xF0;
        }
    }

    Battle_CheckDropChance();
    {
        register void *post_entity asm("$3");
        register void *post_actor asm("$4");
        register u32 callback asm("$2");

        post_entity = D_8009D254;
        post_actor = *(void **)((u8 *)post_entity + 0x238);
        callback = (u32)Entity_CheckActionIdMatch;
        *(u32 *)((u8 *)post_entity + 0x194) = callback;
        D_8009D27C = *(u32 *)((u8 *)post_actor + 0x18) - 0x64;
    }
    Window_SetBoundsByMode(mode_reg & 0xFF);
    {
        register void *final_actor asm("$2");
        register void *final_entity asm("$4");

        final_actor = D_8009D278;
        asm volatile(
            "lui %0, %%hi(D_8009D254)\n"
            "lw %0, %%lo(D_8009D254)(%0)"
            : "=r"(final_entity));
        Entity_SetActionMode(final_entity, *(u8 *)((u8 *)final_actor + 0x12));
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_Update);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepVictory);

void Battle_StepLevelUp(void) {
    u8 step;
    void *entity;
    u32 flags;

    step = D_8009CE74;
    switch (step) {
    case 0:
        entity = D_8009D254;
        if (*(u16 *)((u8 *)entity + 0x16) != 0xA && (D_8009D1A0 & 0x800) == 0) {
            break;
        }
        {
            register u32 entity_flags asm("$2");

            entity_flags = *(u32 *)((u8 *)entity + 0x98);
            *(u32 *)((u8 *)entity + 0x98) = entity_flags | 0x100;
        }
        Pm_StopAllBoth();
        Aya_SetTotalExp(D_8009D304, D_8009D21C, D_800A7FF0_addr);
        Render_BeginSceneLoad();
        D_8009CE74++;
        break;

    case 1:
        if (D_8009D2A4 == 0x3E8) {
            entity = D_8009D254;
            D_8009CE74 = step + 1;
            *(u32 *)((u8 *)entity + 0x98) &= -0x101;
        }
        break;

    case 2:
        entity = D_8009D254;
        if (*(u8 *)((u8 *)entity + 0xF) == *(u16 *)((u8 *)entity + 0x1A)) {
            flags = D_8009D1A0;
            if ((flags & 0x1800) == 0) {
                Entity_SetActionMode(entity, 0x15);
            } else {
                register u32 cleared_flags asm("$2");

                cleared_flags = flags & -0x1801;
                asm volatile(
                    ".set noat\n"
                    "lui $1, %%hi(D_8009D1A0)\n"
                    "sw %0, %%lo(D_8009D1A0)($1)\n"
                    ".set at"
                    :
                    : "r"(cleared_flags)
                    : "$1", "memory");
                Entity_SetActionMode(entity, 0x18);
            }
            D_8009CE74++;
        }
        break;

    case 3:
        if (CD_StepReadState(0) != 1) {
            Battle_SetupPlayerPalette();
            D_8009D28C = 9;
            D_800B0CD8 &= 0xFFFF7FFF;
        }
        break;
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepPostBattle);
