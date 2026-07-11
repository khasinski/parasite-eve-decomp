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
extern u8 D_8009CE70;
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
extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_800BE9F0_o __asm__("D_800BE9F0");
extern struct { char _[16]; } D_800B0E38_o __asm__("D_800B0E38");
extern struct { char _[16]; } D_800B0CEC_o __asm__("D_800B0CEC");
extern struct { char _[16]; } D_800B0D88_o __asm__("D_800B0D88");
extern struct { char _[16]; } D_800B0D8A_o __asm__("D_800B0D8A");
extern struct { char _[16]; } D_800B00EC_o __asm__("D_800B00EC");
extern struct { char _[16]; } D_800B692C_o __asm__("D_800B692C");
extern struct { char _[16]; } D_800B6948_o __asm__("D_800B6948");
#define g_FieldActorListHead (*(void **)&D_8009D20C_o)
#define g_ActiveDrawSlot (*(s32 *)&D_8009CDDC_o)
#define D_800BE9F0 ((u8 *)&D_800BE9F0_o)
#define D_800B0E38 ((u8 **)&D_800B0E38_o)
#define D_800B0CEC ((u8 *)&D_800B0CEC_o)
#define D_800B0D88 (*(u16 *)&D_800B0D88_o)
#define D_800B0D8A (*(u8 *)&D_800B0D8A_o)
#define D_800B00EC ((u8 *)&D_800B00EC_o)
#define D_800B692C ((u8 *)&D_800B692C_o)
#define D_800B6948 ((u8 *)&D_800B6948_o)
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
int Render_StartFadeIn(int frames);
void Render_FadeEntityColor(void *anim, int r, int g, int b);
void Anim_SetInterpRate(void *anim, int rate);
void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2);
void Akao_Cmd_21(int arg0, int arg1);
void AddPrim(void *ot, void *prim);
int rcos(int angle);
void Sys_Shutdown(void);
int Scene_LoadRoomAssets(int room);
void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Aya_DeriveStats(void *src, void *dst);
void Battle_StartDeathAnim(void);

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

static void Battle_VictoryFadeMainAndPartner(int shade) {
    Render_FadeEntityColor((u8 *)D_8009D254 + 0x1B4, shade, shade, shade);
    Render_FadeEntityColor(D_800B0CEC, shade, shade, shade);
}

static void Battle_VictoryMarkEntityFade(void) {
    *(u16 *)((u8 *)D_8009D254 + 0x250) |= 2;
    D_800B0D88 |= 2;
}

static void Battle_VictoryInitStatBars(void) {
    u8 *base;

    base = D_800B00EC;

    base[0x00] = 0;
    base[0x01] = 0x46;
    base[0x02] = 0x82;
    base[0x08] = 0x9F;
    base[0x09] = 0xFF;
    base[0x0A] = 0xF9;

    base[0x10] = 0;
    base[0x11] = 0x46;
    base[0x12] = 0x82;
    base[0x18] = 0x9F;
    base[0x19] = 0xFF;
    base[0x1A] = 0xF9;

    base[0x24] = 0;
    base[0x25] = 0x46;
    base[0x26] = 0x82;
    base[0x2C] = 0x9F;
    base[0x2D] = 0xFF;
    base[0x2E] = 0xF9;

    base[0x34] = 0;
    base[0x35] = 0x46;
    base[0x36] = 0x82;
    base[0x3C] = 0x9F;
    base[0x3D] = 0xFF;
    base[0x3E] = 0xF9;

    base[0x48] = 0;
    base[0x49] = 0x46;
    base[0x4A] = 0x82;
    base[0x50] = 0x9F;
    base[0x51] = 0xFF;
    base[0x52] = 0xF9;

    base[0x38] = 0x4A;
    base[0x4C] = 0x4A;

    D_800B692C[0] = 0x9F;
    D_800B692C[1] = 0xFF;
    D_800B692C[2] = 0xF9;
    D_800B6948[0] = 0x9F;
    D_800B6948[1] = 0xFF;
    D_800B6948[2] = 0xF9;

    D_800B0134 = 0;
    D_800B0135 = 0x82;
    D_800B0136 = 0x36;
    D_800B013C = 0x4A;
    D_800B013D = 0xFF;
    D_800B013E = 0x3B;
    D_800B0144 = 0;
    D_800B0145 = 0x82;
    D_800B0146 = 0x36;
    D_800B014C = 0x4A;
    D_800B014D = 0xFF;
    D_800B014E = 0x3B;
    D_800B017C = 0;
    D_800B017D = 0x82;
    D_800B017E = 0x36;
    D_800B0184 = 0x4A;
    D_800B0185 = 0xFF;
    D_800B0186 = 0x3B;
    D_800B018C = 0;
    D_800B018D = 0x82;
    D_800B018E = 0x36;
    D_800B0194 = 0x4A;
    D_800B0195 = 0xFF;
    D_800B0196 = 0x3B;
}

int Battle_StepVictory(void) {
    void *entity;
    void *actor;
    int ret;
    int shade;
    u8 step;

    ret = 0;
    step = D_8009CE74;

    switch (step) {
    case 0:
        entity = D_8009D254;
        if (*(u8 *)((u8 *)entity + 0xE) != 0x13) {
            Entity_SetActionMode(entity, 0x13);
            entity = D_8009D254;
        }

        if (*(u8 *)((u8 *)entity + 0xF) == *(u16 *)((u8 *)entity + 0x16)) {
            D_8009CE70 = 0x10;
            *(u32 *)((u8 *)entity + 0x98) |= 0x100;
            D_8009CE74++;
        } else {
            *(u32 *)((u8 *)entity + 0x98) &= -0x101;
        }
        break;

    case 1:
        if (D_8009CE70 != 0) {
            shade = (~(D_8009CE70 << 3)) & 0xFF;
            Battle_VictoryFadeMainAndPartner(shade);
            D_8009CE70--;
        } else {
            Battle_VictoryFadeMainAndPartner(0xFF);
            Anim_SetInterpRate((u8 *)D_8009D254 + 0x1B4, 0x1E);
            Battle_VictoryMarkEntityFade();
            Anim_SetInterpRate(D_800B0CEC, 0x1E);
            D_8009CE74++;
        }
        break;

    case 2:
        if (*(u8 *)((u8 *)D_8009D254 + 0x252) == 0 && D_800B0D8A == 0) {
            Scene_LoadRoomAssets(0x69);
            entity = D_8009D254;
            Asset_Find08Alt(0x4AF, 0, *(s16 *)((u8 *)entity + 0x2A),
                            *(s16 *)((u8 *)entity + 0x2E),
                            *(s16 *)((u8 *)entity + 0x32));
            D_8009CE70 = 0x34;
            D_8009CE74++;
        }
        break;

    case 3:
        if (D_8009CE70 != 0) {
            D_8009CE70--;
        } else {
            *(u8 *)((u8 *)D_8009D254 + 0x252) = 1;
            Anim_SetInterpRate((u8 *)D_8009D254 + 0x1B4, 0x1E);
            *(u16 *)((u8 *)D_8009D254 + 0x250) |= 4;
            Render_FadeEntityColor((u8 *)D_8009D254 + 0x1B4, 0xFF, 0xFF, 0xFF);
            D_800B0D8A = 1;
            Anim_SetInterpRate(D_800B0CEC, 0x1E);
            D_800B0D88 |= 4;
            Render_FadeEntityColor(D_800B0CEC, 0xFF, 0xFF, 0xFF);
            Entity_SetActionMode(D_8009D254, 0xF);
            D_8009CE70 = 0x1E;
            *(u32 *)((u8 *)D_8009D254 + 0x14) = 0x230000;
            *(u32 *)((u8 *)D_8009D254 + 0x18) = 0x220000;
            D_8009CE74++;
        }
        break;

    case 4:
        if (D_8009CE70 == 0x10) {
            entity = D_8009D254;
            D_8009CE74++;
            *(u32 *)((u8 *)entity + 0x98) &= -0x101;
        } else {
            D_8009CE70--;
        }
        break;

    case 5:
        entity = D_8009D254;
        if (*(u8 *)((u8 *)entity + 0xF) == *(u16 *)((u8 *)entity + 0x1A)) {
            Entity_SetActionMode(entity, *(u8 *)((u8 *)D_8009D278 + 0x12));
            D_8009CE74++;
        }
        break;

    case 6:
        actor = D_8009D278;
        *(u16 *)((u8 *)actor + 0x0C) = *(u16 *)((u8 *)actor + 0x1C) >> 1;
        *(u32 *)((u8 *)actor + 0x08) >>= 2;
        Battle_ResetEnemyStats(1);
        actor = D_8009D278;
        entity = D_8009D254;
        *(u16 *)((u8 *)actor + 0x58) = *(u16 *)((u8 *)actor + 0x0C);
        *(u16 *)((u8 *)actor + 0x5A) = *(u16 *)((u8 *)entity + 0x210);
        *(u16 *)((u8 *)actor + 0x5C) = *(u16 *)((u8 *)entity + 0x212);
        *(u8 *)((u8 *)actor + 0x5E) = 0x1E;
        *(u8 *)((u8 *)actor + 0x5F) = 1;
        Aya_DeriveStats((u8 *)actor + 0x2C, (u8 *)actor + 0x30);
        Battle_VictoryInitStatBars();
        D_8009CE74 = 0;
        D_8009D28C = 0;
        ret = 1;
        if (D_8009D2A0 == 0) {
            Battle_StartDeathAnim();
        }
        break;

    case 7:
        break;
    }

    return ret;
}

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

static int Battle_PostEntityVisibleForCleanup(void *entity) {
    u8 *core;

    core = *(u8 **)entity;
    if (core != 0) {
        return core[5] != 1;
    }

    return ((*(u32 *)((u8 *)entity + 0x98) & 0x40) == 0);
}

static void Battle_PostBattleSetPrimRect(int x0, int y0, int x1, int y1) {
    u8 *prim;
    int slot;

    slot = g_ActiveDrawSlot;
    prim = D_800BE9F0 + ((slot * 5) << 3);

    *(s16 *)(prim + 0x08) = x0;
    *(s16 *)(prim + 0x0A) = y0;
    *(s16 *)(prim + 0x10) = x1;
    *(s16 *)(prim + 0x12) = y0;
    *(s16 *)(prim + 0x18) = x0;
    *(s16 *)(prim + 0x1A) = y1;
    *(s16 *)(prim + 0x20) = x1;
    *(s16 *)(prim + 0x22) = y1;
}

static u8 *Battle_PostBattleActivePrim(void) {
    int slot;

    slot = g_ActiveDrawSlot;
    return D_800BE9F0 + ((slot * 5) << 3);
}

static void Battle_PostBattleStepTimer(void) {
    D_8009CE70--;
}

void Battle_StepPostBattle(void) {
    void *entity;
    void *iter;
    u8 step;

    step = D_8009CE74;
    switch (step) {
    case 0:
        entity = D_8009D254;
        if (*(u8 *)((u8 *)entity + 0xE) != 0x13) {
            Entity_SetActionMode(entity, 0x13);
            entity = D_8009D254;
        }

        if (*(u8 *)((u8 *)entity + 0xF) == *(u16 *)((u8 *)entity + 0x16)) {
            iter = g_FieldActorListHead;
            while (iter != 0) {
                if (iter != D_8009D254 && Battle_PostEntityVisibleForCleanup(iter)) {
                    u8 *core;
                    u32 flags;

                    *(u16 *)((u8 *)iter + 0x250) |= 2;
                    flags = *(u32 *)((u8 *)iter + 0x98) | 0x1000;
                    *(u32 *)((u8 *)iter + 0x98) = flags;
                    core = *(u8 **)iter;
                    if (core != 0) {
                        Entity_SetActionMode(iter, (u16)(s8)core[6]);
                        flags = *(u32 *)((u8 *)iter + 0x98);
                        if ((flags & 0x40000000) && core[0xAF] == 0) {
                            *(u32 *)((u8 *)iter + 0x98) = flags | 0x10;
                            *(u32 *)iter = 0;
                        }
                    }

                    *(u32 *)((u8 *)iter + 0x68) = 0;
                    *(u32 *)((u8 *)iter + 0x6C) = 0;
                    *(u32 *)((u8 *)iter + 0x70) = 0;
                }
                iter = *(void **)((u8 *)iter + 4);
            }

            Battle_ResetEnemyStats(0);
            D_8009CE70 = 0x46;
            D_8009CE74++;
            *(u32 *)((u8 *)D_8009D254 + 0x98) |= 0x100;
        } else {
            *(u32 *)((u8 *)entity + 0x98) &= -0x101;
        }
        break;

    case 1:
        if (D_8009CE70 == 0x3C) {
            Render_StartFadeIn(0x3C);
            Akao_Cmd_C1_WithSlot(0, 0x3C, 0);

            iter = g_FieldActorListHead;
            while (iter != 0) {
                if (iter != D_8009D254 && Battle_PostEntityVisibleForCleanup(iter)) {
                    Anim_SetInterpRate((u8 *)iter + 0x1B4, 0x3C);
                }
                iter = *(void **)((u8 *)iter + 4);
            }
        } else {
            iter = g_FieldActorListHead;
            while (iter != 0) {
                if (iter != D_8009D254) {
                    u8 *core = *(u8 **)iter;
                    if ((core != 0 || ((*(u32 *)((u8 *)iter + 0x98) & 0x40) == 0)) &&
                        *(u8 *)((u8 *)iter + 0x252) == 0) {
                        *(u32 *)((u8 *)iter + 0x98) |= 0x10;
                    }
                }
                iter = *(void **)((u8 *)iter + 4);
            }
        }

        if (D_8009CE70 != 0) {
            Battle_PostBattleStepTimer();
        } else {
            Pm_StopAllBoth();
            Akao_Cmd_21(0, 0xFF);
            D_8009CE70 = 0x1E;
            D_8009CE74++;
        }
        break;

    case 2: {
        int timer;
        int x0;
        int x1;

        timer = D_8009CE70;
        x0 = 0x78 - timer * 4;
        x1 = 0xDC + timer * 4;
        Battle_PostBattleActivePrim()[4] = x0;
        Battle_PostBattleActivePrim()[5] = x0;
        Battle_PostBattleActivePrim()[6] = x0;
        Battle_PostBattleSetPrimRect(0x64 - timer * 4, 0x7A, x1, 0x7C);

        if (timer != 0) {
            Battle_PostBattleStepTimer();
        } else {
            D_8009CE70 = 0x50;
            D_8009CE74++;
        }
        break;
    }

    case 3: {
        int timer;
        int y;

        timer = D_8009CE70;
        if ((u8)timer >= 0x1A) {
            int pulse;

            pulse = rcos(((u8)timer - 0x10) << 4);
            pulse = (unsigned int)(pulse * 11) >> 11;
            y = 0x7A - (pulse & 0xFF);
            Battle_PostBattleSetPrimRect(0x64, y, 0xDC, 0x7C);
        } else {
            Battle_PostBattleSetPrimRect(0x64, 0x64, 0xDC, 0x7C);
        }

        if (D_8009CE70 != 0) {
            Battle_PostBattleStepTimer();
        } else {
            Anim_SetInterpRate((u8 *)D_8009D254 + 0x1B4, 0x3C);
            *(u16 *)((u8 *)D_8009D254 + 0x250) |= 2;
            Anim_SetInterpRate(D_800B0CEC, 0x3C);
            D_8009CE70 = 0x3C;
            D_800B0D88 |= 2;
            D_8009CE74++;
        }
        break;
    }

    case 4:
        if (*(u8 *)((u8 *)D_8009D254 + 0x252) == 0 && D_800B0D8A == 0) {
            D_8009CE74++;
        } else {
            u8 shade;

            shade = D_8009CE70 * 2;
            Battle_PostBattleActivePrim()[4] = shade;
            Battle_PostBattleActivePrim()[5] = shade;
            Battle_PostBattleActivePrim()[6] = shade;
            Battle_PostBattleSetPrimRect(0x64, 0x64, 0xDC, 0x7C);
            Battle_PostBattleStepTimer();
        }
        break;

    case 5:
        Battle_SetupPlayerPalette();
        D_8009D28C = -1;
        Sys_Shutdown();
        break;
    }

    AddPrim(D_800B0E38[g_ActiveDrawSlot] + 0x10, Battle_PostBattleActivePrim());
}
