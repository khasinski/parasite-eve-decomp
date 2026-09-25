/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
#include "pe1/akao.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
u8 D_8009D2D8, D_8009CE3C, D_8009CE60, D_8009D25C;
/* Signed view of the target-mode byte at D_8009CE40. */
s8 g_ItemTargetMode asm("D_8009CE40");
u16 D_8009CE50;
s8 g_BattleTargetIndex;

static inline void StoreSlot(BattleInitSlot *slot, BattleEntity *actor, s16 kind, s16 turn) {
    slot->actor = actor;
    slot->field04 = kind;
    slot->field06 = turn;
}
static inline void SetSlot(int index, BattleEntity *actor, s16 kind, s16 turn) {
    StoreSlot(&D_800BE830[index], actor, kind, turn);
}
static inline void Sound(int command) {
    void *volatile *slot = &g_AkaoBgmHandle;
    if (*slot)
        Akao_SendTableCommand(*slot, command, 0, 128, 127);
}
int Battle_StepEnemyTurn(s8 mode) {
    if (mode == 1) {
        int command = D_8009D2A4;
        s16 queuedCommand = D_8009D2A4;
        D_8009CE60 = 0;
        if (command > 0) {
            if (command < 387) {
                SetSlot(D_8009CE3C, D_8009D254, queuedCommand, (s8)D_8009D2D8);
                D_8009CE3C++;
                D_8009D2D8--;
                g_ItemTargetMode = 0;
                mode = 0;
            } else if (command < 407) {
                switch (command) {
                case 393:
                    Battle_InitFadeVars();
                    g_ItemTargetMode = 4;
                    mode++;
                    break;
                case 394:
                    Battle_InitFadeVars();
                    g_ItemTargetMode = 5;
                    mode++;
                    break;
                case 395:
                    Battle_InitFadeVars();
                    g_ItemTargetMode = 6;
                    mode++;
                    break;
                case 397:
                    Battle_InitFadeVars();
                    g_ItemTargetMode = 7;
                    mode++;
                    break;
                case 406:
                    Battle_InitFadeVars();
                    D_8009D25C = 0;
                    for (mode = 0; (u8)mode < 7; mode++) {
                        int index = D_8009CE3C;
                        int randomValue = rand();
                        SetSlot(index, g_BattleTargetList[randomValue % D_8009D2B0].actor,
                                D_8009D2A4, (s8)D_8009D2D8);
                        D_8009CE3C++;
                    }
                    D_8009D2D8--;
                    g_ItemTargetMode = 0;
                    mode = 0;
                    break;
                default:
                    SetSlot(D_8009CE3C, D_8009D254, D_8009D2A4, (s8)D_8009D2D8);
                    D_8009D2D8--;
                    D_8009CE3C++;
                    g_ItemTargetMode = 0;
                    mode = 0;
                    break;
                }
            } else if (command < 409) {
                SetSlot(D_8009CE3C, D_8009D254, queuedCommand, (s8)D_8009D2D8);
                if (!(g_GameStateFlags & 2)) {
                    D_8009D278 = D_8009D254->core;
                    Asset_Find08Alt(0x453, 1, (s16)D_8009D254->posX.parts.integer,
                                    (s16)D_8009D254->posY.parts.integer,
                                    (s16)D_8009D254->posZ.parts.integer);
                }
                switch (command) {
                case 407:
                    Battle_CopyPadStateToRecord();
                    Battle_SetupEnemyAnims();
                    D_8009D278->actionMode12 = 13;
                    break;
                case 408:
                    Battle_SyncEnemyAttributes();
                    break;
                }
                D_8009CE3C++;
                D_8009D2D8 = 0;
                g_ItemTargetMode = 0;
                mode = -1;
            } else {
                SetSlot(D_8009CE3C, D_8009D254, queuedCommand, (s8)D_8009D2D8);
                D_8009CE3C++;
                D_8009D2D8 = 0;
                g_ItemTargetMode = 0;
                mode = -1;
            }
            if (!g_ItemTargetMode)
                Sound(0x44C);
            D_8009CE50 = D_8009D2A4;
        } else if (command == -1)
            mode = 0;
    } else if (mode == 2) {
        Battle_CycleTarget(g_ItemTargetMode);
        if (D_8009D1F4 & 0x200) {
            SetSlot(D_8009CE3C, g_BattleTargetList[g_BattleTargetIndex].actor, D_8009CE50,
                    (s8)D_8009D2D8);
            D_8009D2D8--;
            D_8009CE3C++;
            g_ItemTargetMode = 0;
            mode = 0;
            {
                void *volatile *slot = &g_AkaoBgmHandle;
                if (*slot) {
                    register int command asm("$5") = 0x44C;
                    register int unused asm("$6") = 0;
                    void *table;
                    table = *slot;
                    asm volatile("" : : "r"(table));
                    Akao_SendTableCommand(table, command, unused, 128, 127);
                }
            }
        } else if (D_8009D1F4 & 0x400) {
            mode = 0;
            BattleCmd_UndoPending();
            /* Keep the mode update in the call delay slot. */
            asm volatile("" : : "r"(mode));
            g_BattleTargetIndex = 0;
            Sound(0x44D);
        }
    }
    return mode;
}
