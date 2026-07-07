/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef unsigned char u8;

extern int g_BattleEntitySlotInUse[][0x37];
u8 D_8009D2A0;
u8 g_EntityInstanceCounter;

void Battle_SaveAyaState(void);
void Save_ResetGlobalFlags(void);

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

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_ResetEnemyStats);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_SetupPlayerPalette);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StartEncounter);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_Update);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepVictory);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepLevelUp);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/battle3", Battle_StepPostBattle);
