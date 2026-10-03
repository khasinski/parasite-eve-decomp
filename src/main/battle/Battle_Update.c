/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_update.h"

#define BATTLE_HUD_SLOT_BYTE(symbol, stride) \
    (*(&(symbol).bytes[0] + (g_BattleTurnDrawSlotView[0] * (stride))))

void Battle_Update(void) {
    /* Retail reserves 0x1A0 unused bytes; original local layout is unknown. */
    volatile u8 matchingStackReserve[0x1A0];
    s32 menuContext;
    s32 menuMode;
    BattleEntity *player;
    Combatant *active;
    s8 menuResult;
    int phase;
    int opened;
    u8 previousPhase;
    Combatant *status;
    BattleEntity *entity;
    BattleEntity *victoryEntity;

    menuMode = 1;
    if (D_8009D278->stateFlags & 0x80000) {
        if (D_8009D28C == 6) {
            D_8009CE7C = 6;
            D_8009D28C = 0;
        }
        if (!(D_8009D278->stateFlags & 0x80000)) {
            goto restorePhase;
        }
    } else {
    restorePhase:
        previousPhase = D_8009CE7C;
        if (previousPhase == 6) {
            D_8009CE7C = 0;
            D_8009D28C = (s32) previousPhase;
        }
    }
    active = (g_BattleTurnPlayerView[0])->core;
    (D_8009D230[0]) = 0;
    D_8009D278 = active;
    D_8009D2A4 = Menu_RunFrameWithArg((int)&D_800A76D8);
    if (D_8009D28C == 0 && D_8009D244 != 0) {
        if ((u16) D_8009D278->hpAlive >= 0x2328U) {
            D_8009D278->hpAlive = 0x2328U;
            if (D_8009D288 == 0) {
                srand((g_BattleTurnFrameCounterView[0]));
                if ((D_800B0E08[0]) != 0) {
                    Akao_SendTableCommand((D_800B0E08[0]), 0x454, 0, 0x80, 0x7F);
                }
                D_8009D288 = 1;
            }
            if (!(D_8009D278->stateFlags & 0x2000) && (g_BattleUpdatePaletteEnabled != 0)) {
                if (((g_BattleTurnFrameCounterView[0]) & 3) == 0) {
                    register int topGreen asm("$8") = 0x46;
                    register int topBlue asm("$7") = 0x82;
                    register int bottomRed asm("$6") = 0x9F;
                    register int bottomGreen asm("$5") = 0xFF;
                    int bottomBlue = 0xF9;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
                    asm volatile("" : : "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = bottomRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = bottomRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = bottomRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
                    asm("" : : "i"(0));
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
                } else if (((g_BattleTurnFrameCounterView[0]) & 3) == 1) {
                    register int red asm("$5") = 0x50;
                    register int green asm("$6") = 0xA3;
                    int blue = 0xBE;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
                    asm volatile("" : : "r"(red), "r"(green), "r"(blue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
                    asm("" : : "i"(1));
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
                } else if (((g_BattleTurnFrameCounterView[0]) & 3) == 2) {
                    register int topRed asm("$7") = 0x9F;
                    register int topGreen asm("$8") = 0xFF;
                    register int topBlue asm("$6") = 0xF9;
                    register int bottomGreen asm("$5") = 0x46;
                    int bottomBlue = 0x82;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = topRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
                    asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomGreen), "r"(bottomBlue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = topRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
                    asm("" : : "i"(2));
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
                } else if (((g_BattleTurnFrameCounterView[0]) & 3) == 3) {
                    int red = 0x50;
                    register int green asm("$6") = 0xA3;
                    int blue = 0xBE;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
                    asm volatile("" : : "r"(red), "r"(green), "r"(blue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
                }
                if ((Pad_GetMenuPressedBitOrDisabled() << 0x18) == 0) {
                    opened = 0;
                    if ((D_8009D1F4[0]) & 0x200) {
                        (D_8009D1F0[0]) = 0;
                        opened = 1;
                    } else if ((D_8009D1F4[0]) & 0x80) {
                        opened = 1;
                        (D_8009D1F0[0]) = opened;
                        Inventory_OpenAyaItemList(1);
                        Render_BeginSceneLoad();
                    }
                    if ((u8)opened != 0) {
                        Akao_Cmd_21(0, 0xFF);
                        D_8009D28C = 1;
                        (D_8009D290[0]) = 0;
                        g_BattleTurnGameFlagsWrite[0] = g_BattleTurnGameFlagsRead[0] | 4;
                        menuMode = 0;
                        D_8009D2B0 = Battle_BuildTargetList();
                        Battle_Init();
                        menuContext = 1;
                        if (D_8009D2B0 != 0) {
                            menuContext = 0;
                            /* Preserve the retail branch instead of folding to SLTIU. */
                            asm("" : : "r"(menuContext));
                        }
                        Menu_SetItemContext(menuContext);
                    }
                }
            }
        }
        if ((Pad_GetMenuPressedBitOrDisabled() << 0x18) > 0) {
            if (!(D_8009D278->stateFlags & 0x4000) || ((g_BattleTurnGameFlagsRead[0]) & 0x100)) {
                menuMode = 2;
                Battle_StepScriptEntry();
            }
            if ((D_8009D294[0]) != 0) {
                Battle_ResolveHitOnTimer();
            }
        }
        if ((D_8009D1AC[0]) & 0x300) {
            Save_DrawSlotMetadata();
        }
        Menu_MainUpdate(menuMode);
        entity = (g_BattleTurnEntityListView[0]);
        if (entity != 0) {
            do {
                if ((entity != (g_BattleTurnPlayerView[0])) && (entity->core != 0)) {
                    Battle_UpdateEnemy(entity);
                }
                entity = entity->next;
            } while (entity != 0);
        }
        if (D_8009D23C != 0) {
            Pm_SendCmd(D_8009D2FC, 0, 1, (int)&(D_800B8A90[0]), 0, 0);
            Pm_SendCmd(D_8009D2FC, 0, 0, 1, 0, 0);
            D_8009D23C = 0;
        }
        if ((D_8009D235[0]) != 0) {
            Gpu_QueuePrimitive();
        }
        player = g_BattleTurnPlayerView[0];
        if ((player->animLastFrame == player->animPrev.parts.integer) && ((u8)player->actionMode < 4U)) {
            if ((g_BattleTurnResumeCountRead[0]) == 0) {
                Entity_SetActionMode(player, D_8009D278->actionMode12);
                if (!((D_800B0CE6.bytes[0]) & 1) && ((Pad_GetMenuPressedBitOrDisabled() << 0x18) == 0) && (D_8009D28C == 0) && (D_8009D278->action->turnWord & 0x8000) && (D_8009D1D0 != 0)) {
                    Battle_RollEnemySpawn(D_8009D1D0);
                    D_8009D1D0 = 0;
                }
            } else {
                if ((u16) (g_BattleTurnResumeCountRead[0]) >= 2U) {
                    player->entityFlags |= 0x100;
                }
                g_BattleTurnResumeCountWrite[0] = 0;
                Entity_SetActionMode(g_BattleTurnActionPlayerView[0], (g_BattleTurnResumeModeView[0]));
                player = g_BattleTurnResumedPlayerView[0];
                player->animFrame = g_BattleTurnPreviousFrameView[0];
                player->animPrev.fixed = g_BattleTurnResumeFrameView[0] + 0xFFFF0000;
            }
        }
        if (((g_BattleTurnPlayerView[0])->actionMode == 0xD) && !((D_800B0CE6.bytes[0]) & 1)) {
            D_8009D278->actionMode12 = 4U;
            if ((g_BattleTurnPlayerView[0])->entityFlags & 0x100) {
                Asset_Find08Alt(0x453, 0, (g_BattleTurnPlayerView[0])->posX.parts.integer, (g_BattleTurnPlayerView[0])->posY.parts.integer, (s32) (g_BattleTurnPlayerView[0])->posZ.parts.integer);
                (g_BattleTurnPlayerView[0])->entityFlags = (s32) ((g_BattleTurnPlayerView[0])->entityFlags & ~0x100);
            }
            player = g_BattleTurnPlayerView[0];
            if (player->animLastFrame == player->animPrev.parts.integer) {
                Entity_SetActionMode(player, D_8009D278->actionMode12);
                g_BattleTurnMoveLockWrite[0] = g_BattleTurnMoveLockRead[0] & ~1;
            }
        }
        D_8009D1E8 += 1;
    } else {
        phase = D_8009D28C;
        if (phase == 1) {
            menuResult = Battle_HandleItemMenu();
            D_8009D28C = (s32) menuResult;
            if (menuResult == 0) {
                g_BattleTurnGameFlagsWrite[0] = g_BattleTurnGameFlagsRead[0] & ~4;
            }

        } else if (phase == 2) {
            Battle_StepLevelUp();

        } else if (phase == 3) {
            if (D_8009D278->stateFlags & 0x800) {
                if (Battle_StepVictory() & 0xFF) {
                    D_8009D278->stateFlags = (s32) (D_8009D278->stateFlags & ~0x800);
                }
            } else if (Inv_CountByValue(0x12) != 0) {
                if (Battle_StepVictory() & 0xFF) {
                    Inv_FindItemById(0x12);
                }
            } else {
                Battle_StepPostBattle();
            }
            status = D_8009D278;
            if (status->panelA_timer != 0) {
                Battle_DrawStatusPanel(0, &status->panelA_val);
                D_8009D278->panelA_timer--;
                status = D_8009D278;
            }
            if (status->panelAux_timer != 0) {
                Battle_DrawStatusPanel(0, &status->panelAux_val);
                D_8009D278->panelAux_timer = (u8) (D_8009D278->panelAux_timer - 1);
            }
            victoryEntity = (g_BattleTurnEntityListView[0]);
            if (victoryEntity != 0) {
            updateVictoryEntity:
                if ((victoryEntity != (g_BattleTurnPlayerView[0])) && (victoryEntity->core != 0)) {
                    Battle_UpdateEnemy(victoryEntity);
                }
                victoryEntity = victoryEntity->next;
                if (victoryEntity != 0) {
                    goto updateVictoryEntity;
                }
            }

        } else if (phase == 4) {
            Battle_StepEscapeOrDeath();

        } else if (phase == 5) {
            Battle_StepEntityDeath();

        } else if (phase == 6) {
            Battle_PhaseInitEnemyTurn();

        } else if (phase == 7) {
            Battle_PhaseHitReaction();

        } else if (phase == 8) {
            Battle_PhaseEndTurn();

        }
    }
    if (((D_8009D1CE[0]) != 0) && (D_8009D28C == 0)) {
        Menu_SaveOverlayDraw();
    }
    if (D_8009D244 != 0) {
        Battle_DrawActiveStatus();
    }
    if (D_8009D2A4 != 0) {
        Render_BeginSceneLoad();
    }
}
