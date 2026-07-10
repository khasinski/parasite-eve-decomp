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

void Battle_SaveAyaState(void);
void Save_ResetGlobalFlags(void);
void Battle_FlushScriptSounds(void);
void Tbl_ResetAll(void);

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

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_SetupPlayerPalette);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StartEncounter);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_Update);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepVictory);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepLevelUp);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepPostBattle);
