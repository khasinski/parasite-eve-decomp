/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_turn.h"

#define BATTLE_HUD_BYTE(symbol) ((symbol).bytes[0])
#define BATTLE_HUD_SLOT_BYTE(symbol, stride) \
    (*(&BATTLE_HUD_BYTE(symbol) + (g_BattleTurnDrawSlotView[0] * (stride))))

/* Initialize the enemy turn and wait for HUD, animation and texture work. */
void Battle_PhaseInitEnemyTurn(void) {
    /* Retail reserves this unused local area; its original purpose is unknown. */
    volatile u8 matchingStackReserve[0x300];
    s32 entityFlags;
    u8 ready;
    Combatant *actor;
    EnemyCombatant *enemy;
    BattleEntity *entity;
    BattleEntity *player;
    Combatant *active;
    BattleEntity *initialPlayer;
    register s32 moveLock asm("$3");

    register s32 value0 asm("$2");
    register s32 value1 asm("$3");
    register s32 brightGreen asm("$4");
    s32 color1;
    s32 blue;
    s32 color2;

    u32 hp;
    s32 gameFlags;
    s32 clearMask;
    register Combatant *finishActor asm("$2");
    /* Clear prior script effects and stop the player before waiting for actors. */
    Battle_FlushScriptSounds();
    ready = 1;
    Tbl_ResetAll();
    moveLock = (g_BattleTurnMoveLockRead[0]) | 1;
    g_BattleTurnMoveLockWrite[0] = moveLock;
    initialPlayer = g_BattleTurnPlayerView[0];
    active = D_8009D278;
    clearMask = ~4;
    initialPlayer->motionX = 0;
    initialPlayer->motionY = 0;
    initialPlayer->motionZ = 0;
    gameFlags = (g_BattleTurnGameFlagsRead[0]);
    hp = active->hpAlive;
    g_BattleTurnGameFlagsWrite[0] = gameFlags & clearMask;
    /* Animate the charged gauge and status highlight using the draw slot. */
    if (hp >= 0x2328U) {
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
    }
    if (D_8009D278->stateFlags & 0x2000) {
        if (((g_BattleTurnFrameCounterView[0]) & 3) == 0) {
            register int topRed asm("$9") = 0xFF;
            register int topGreen asm("$8") = 0x3D;
            register int topBlue asm("$7") = 0x81;
            register int bottomRed asm("$6") = 0x83;
            register int bottomGreen asm("$5") = 0x13;
            int bottomBlue = 1;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = topRed;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = bottomGreen;
            asm("" : : "i"(0));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = bottomBlue;
        } else if (((g_BattleTurnFrameCounterView[0]) & 3) == 1) {
            register int red asm("$6") = 0xC1;
            register int green asm("$5") = 0x28;
            int blue = 0x41;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = red;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = green;
            asm("" : : "i"(1));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = blue;
        } else if (((g_BattleTurnFrameCounterView[0]) & 3) == 2) {
            register int topRed asm("$9") = 0x83;
            register int topGreen asm("$8") = 0x13;
            register int topBlue asm("$7") = 1;
            register int bottomRed asm("$6") = 0xFF;
            register int bottomGreen asm("$5") = 0x3D;
            int bottomBlue = 0x81;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = topRed;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = bottomGreen;
            asm("" : : "i"(2));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = bottomBlue;
        } else if (((g_BattleTurnFrameCounterView[0]) & 3) == 3) {
            register int red asm("$6") = 0xC1;
            register int green asm("$5") = 0x28;
            int blue = 0x41;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = red;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = blue;
        }
    }
    /* Floating status panels must finish before the next phase. */
    actor = D_8009D278;
    if (actor->panelA_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&actor->panelA_val);
        ready = 0;
        D_8009D278->panelA_timer = (u8) (D_8009D278->panelA_timer - 1);
    }
    actor = D_8009D278;
    if (actor->panelB_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&actor->panelB_val);
        ready = 0;
        D_8009D278->panelB_timer = (u8) (D_8009D278->panelB_timer - 1);
    }
    if (D_8009D278->panelAux_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&D_8009D278->panelAux_val);
        ready = 0;
        D_8009D278->panelAux_timer = (u8) (D_8009D278->panelAux_timer - 1);
    }
    /* Wait for player/enemy animations to reach their requested idle modes. */
    entity = g_BattleTurnEntityListView[0];
    if (entity != 0) {
        do {
            if (entity == g_BattleTurnPlayerView[0]) {
                if (entity->actionMode != D_8009D278->actionMode12) {
                    ready = 0;
                    entityFlags = entity->entityFlags & ~0x100;
                    entity->entityFlags = entityFlags;
                    if (entity->animLastFrame == entity->animPrev.parts.integer) {
                        if (((u8) entity->actionMode < 4U) && ((g_BattleTurnResumeCountRead[0]) != 0)) {
                            if ((u16) (g_BattleTurnResumeCountRead[0]) >= 2U) {
                                entity->entityFlags = (s32) (entityFlags | 0x100);
                            }
                            g_BattleTurnResumeCountWrite[0] = 0;
                            Entity_SetActionMode(g_BattleTurnActionPlayerView[0], (g_BattleTurnResumeModeView[0]));
                            player = g_BattleTurnResumedPlayerView[0];
                            player->animFrame = (s32) (g_BattleTurnResumeFrameView[0]);
                            player->animPrev.fixed = (s32) (g_BattleTurnPreviousFrameView[0] + 0xFFFF0000);
                        } else {
                            Entity_SetActionMode(entity, D_8009D278->actionMode12);
                        }
                    }
                }
            } else {
                enemy = entity->core;
                if ((enemy != 0) && ((s8)enemy->field04.bytes.field05 != 1)) {
                    if ((u32) (entity->actionMode - 2) >= 2U) {
                        entity->entityFlags = (s32) (entity->entityFlags & ~0x1000);
                        Entity_TickAnimSequences(entity);
                        ready = 0;
                        if (entity->animLastFrame == entity->animPrev.parts.integer) {
                            Entity_SetActionMode(entity, (u16)(s8)((EnemyCombatant *)entity->core)->field06.bytes.low);
                        } else if (entity->animStep != 0x10000) {
                            entity->animStep = 0x10000;
                        }
                    }
                    Battle_UpdateEnemy(entity);
                }
            }
            entity = entity->next;
        } while (entity != 0);
    }
    /* Texture uploads can keep the phase pending after animations settle. */
    if (Asset_LoadTimTextures(0) != 0) {
        ready = 0;
    }
    if ((u8)ready != 0) {
        Akao_Cmd_21(0, 0xFF);
        blue = 0x82;
        color2 = 0x9F;
        brightGreen = 0xFF;
        color1 = 0xF9;
        finishActor = D_8009D278;
        finishActor->knockbackDistance = 0;
        finishActor->knockbackFrames = 0;
        value0 = 7;
        D_8009D28C = value0;
        value0 = 0x46;
        BATTLE_HUD_BYTE(D_800B00ED) = value0;
        BATTLE_HUD_BYTE(D_800B00FD) = value0;
        BATTLE_HUD_BYTE(D_800B0111) = value0;
        BATTLE_HUD_BYTE(D_800B0121) = value0;
        value0 = (g_BattleTurnMoveLockRead[0]);
        value1 = ~1;
        BATTLE_HUD_BYTE(D_800B00EC) = 0;
        BATTLE_HUD_BYTE(D_800B00EE) = blue;
        BATTLE_HUD_BYTE(D_800B00F4) = color2;
        BATTLE_HUD_BYTE(D_800B00F5) = brightGreen;
        BATTLE_HUD_BYTE(D_800B00F6) = color1;
        BATTLE_HUD_BYTE(D_800B00FC) = 0;
        BATTLE_HUD_BYTE(D_800B00FE) = blue;
        BATTLE_HUD_BYTE(D_800B0104) = color2;
        BATTLE_HUD_BYTE(D_800B0105) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0106) = color1;
        BATTLE_HUD_BYTE(D_800B692C) = color2;
        BATTLE_HUD_BYTE(D_800B692D) = brightGreen;
        BATTLE_HUD_BYTE(D_800B692E) = color1;
        BATTLE_HUD_BYTE(D_800B0110) = 0;
        BATTLE_HUD_BYTE(D_800B0112) = blue;
        BATTLE_HUD_BYTE(D_800B0118) = color2;
        BATTLE_HUD_BYTE(D_800B0119) = brightGreen;
        BATTLE_HUD_BYTE(D_800B011A) = color1;
        BATTLE_HUD_BYTE(D_800B0120) = 0;
        BATTLE_HUD_BYTE(D_800B0122) = blue;
        BATTLE_HUD_BYTE(D_800B0128) = color2;
        BATTLE_HUD_BYTE(D_800B0129) = brightGreen;
        BATTLE_HUD_BYTE(D_800B012A) = color1;
        g_BattleTurnMoveLockWrite[0] = value0 & value1;
        BATTLE_HUD_BYTE(D_800B6948) = color2;
        BATTLE_HUD_BYTE(D_800B694A) = color1;

        color1 = 0x36;
        value1 = 0x4A;
        value0 = 0x3B;
        color2 = 0x3D;
        BATTLE_HUD_BYTE(D_800B0135) = blue;
        BATTLE_HUD_BYTE(D_800B0145) = blue;
        BATTLE_HUD_BYTE(D_800B017D) = blue;
        BATTLE_HUD_BYTE(D_800B018D) = blue;
        blue = 0x81;
        BATTLE_HUD_BYTE(D_800B0136) = color1;
        BATTLE_HUD_BYTE(D_800B0146) = color1;
        BATTLE_HUD_BYTE(D_800B017E) = color1;
        BATTLE_HUD_BYTE(D_800B018E) = color1;
        color1 = 0x83;
        BATTLE_HUD_BYTE(D_800B013C) = value1;
        BATTLE_HUD_BYTE(D_800B014C) = value1;
        BATTLE_HUD_BYTE(D_800B0184) = value1;
        BATTLE_HUD_BYTE(D_800B0194) = value1;
        value1 = 0x13;
        BATTLE_HUD_BYTE(D_800B013E) = value0;
        BATTLE_HUD_BYTE(D_800B014E) = value0;
        BATTLE_HUD_BYTE(D_800B0186) = value0;
        BATTLE_HUD_BYTE(D_800B0196) = value0;
        value0 = 1;
        BATTLE_HUD_BYTE(D_800B6949) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0134) = 0;
        BATTLE_HUD_BYTE(D_800B013D) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0144) = 0;
        BATTLE_HUD_BYTE(D_800B014D) = brightGreen;
        BATTLE_HUD_BYTE(D_800B017C) = 0;
        BATTLE_HUD_BYTE(D_800B0185) = brightGreen;
        BATTLE_HUD_BYTE(D_800B018C) = 0;
        BATTLE_HUD_BYTE(D_800B0195) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0158) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0159) = color2;
        BATTLE_HUD_BYTE(D_800B015A) = blue;
        BATTLE_HUD_BYTE(D_800B0160) = color1;
        BATTLE_HUD_BYTE(D_800B0161) = value1;
        BATTLE_HUD_BYTE(D_800B0162) = value0;
        BATTLE_HUD_BYTE(D_800B0168) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0169) = color2;
        BATTLE_HUD_BYTE(D_800B016A) = blue;
        BATTLE_HUD_BYTE(D_800B0170) = color1;
        BATTLE_HUD_BYTE(D_800B0171) = value1;
        BATTLE_HUD_BYTE(D_800B0172) = value0;
        BATTLE_HUD_BYTE(D_800B01A0) = brightGreen;
        BATTLE_HUD_BYTE(D_800B01A1) = color2;
        BATTLE_HUD_BYTE(D_800B01A2) = blue;
        BATTLE_HUD_BYTE(D_800B01A8) = color1;
        BATTLE_HUD_BYTE(D_800B01A9) = value1;
        BATTLE_HUD_BYTE(D_800B01AA) = value0;
        BATTLE_HUD_BYTE(D_800B01B0) = brightGreen;
        BATTLE_HUD_BYTE(D_800B01B1) = color2;
        BATTLE_HUD_BYTE(D_800B01B2) = blue;
        BATTLE_HUD_BYTE(D_800B01B8) = color1;
        BATTLE_HUD_BYTE(D_800B01B9) = value1;
        BATTLE_HUD_BYTE(D_800B01BA) = value0;
    }
}

#undef BATTLE_HUD_SLOT_BYTE
#undef BATTLE_HUD_BYTE
