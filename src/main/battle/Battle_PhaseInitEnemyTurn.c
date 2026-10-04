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

void Battle_PhaseHitReaction(void) {
    volatile u8 matchingStackReserve[0x1A0];
    Combatant *active;
    BattleEntity *player;
    BattleEntity *entity;
    EnemyCombatant *enemy;
    if (D_8009D278->hpAlive >= 0x2328U) {
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
    active = D_8009D278;
    if (active->panelA_timer) {
        if (active->panelA_timer == 30) {
            BattleEntity *panelPlayer = g_BattleTurnPlayerView[0];
            active->panelA_x = panelPlayer->renderObject.projected_x;
            active->panelA_y = panelPlayer->renderObject.projected_y;
        }
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&D_8009D278->panelA_val);
        D_8009D278->panelA_timer--;
    }
    active = D_8009D278;
    if (active->panelB_timer) {
        if (active->panelB_timer == 30) {
            player = g_BattleTurnPlayerView[0];
            active->panelB_x = player->renderObject.projected_x;
            active->panelB_y = player->renderObject.projected_y;
            active->panelB_flag = 1;
        }
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&D_8009D278->panelB_val);
        D_8009D278->panelB_timer--;
    }
    for (entity = g_BattleTurnEntityListView[0]; entity; entity = entity->next) {
        if (entity != g_BattleTurnPlayerView[0] && entity->core) {
            enemy = entity->core;
            if (enemy->coreFlags & 0x6000) {
                Battle_ProcessActionSlot(entity);
                if ((enemy->coreFlags & 0x6000) == 0x2000)
                    enemy->coreFlags = (enemy->coreFlags & ~0x6000) | 0x4000;
            }
            if (enemy->panelC_timer) {
                if (enemy->panelC_timer == 30) {
                    enemy->panelC_x = entity->renderObject.projected_x;
                    enemy->panelC_y = entity->renderObject.projected_y;
                }
                Battle_DrawStatusPanel(1, (BattleStatusPanel *)&enemy->panelC_val);
                enemy->panelC_timer--;
            }
        }
    }
}

void Battle_PhaseEndTurn(void) {
    /* Retail reserves 0x300 unused bytes; original local layout is unknown. */
    volatile u8 matchingStackReserve[0x300];
    BattleEntity *player;
    Combatant *active;
    BattleEntity *initialPlayer;
    BattleEntity *entity;
    EnemyCombatant *enemy;
    u8 ready;
    u8 mode;
    u8 step;
    u8 nextMode;
    u32 hp;
    register s32 c2 asm("$2");
    s32 c3;
    s32 c4;
    register s32 c5 asm("$6");
    s32 c6;
    register s32 c7 asm("$8");

    initialPlayer = g_BattleTurnPlayerView[0];
    active = D_8009D278;
    initialPlayer->motionX = 0;
    initialPlayer->motionY = 0;
    initialPlayer->motionZ = 0;
    hp = active->hpAlive;
    ready = 1;
    /* Animate the charged gauge and active status colors. */
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
    player = g_BattleTurnActionPlayerView[0];
    mode = D_8009D278->actionMode12;
    if (player->actionMode != mode) {
        Entity_SetActionMode(player, mode);
    }
    step = D_8009CE74;
    switch (step) {
    case 0:
        /* Wait for enemy actions to finish before stopping effects. */
        entity = g_BattleTurnEntityListView[0];
        g_BattleTurnMoveLockWrite[0] = g_BattleTurnMoveLockRead[0] | 1;
        if (entity != 0) {
            do {
                if (entity != g_BattleTurnPlayerView[0]) {
                    enemy = entity->core;
                    if ((enemy != 0) && ((s8)enemy->field04.bytes.field05 != 1)) {
                        if ((u32) (entity->actionMode - 2) >= 2U) {
                            if (entity->animLastFrame == entity->animPrev.parts.integer) {
                                Entity_SetActionMode(entity, (u16)(s8)enemy->field06.bytes.low);
                            } else if (entity->animStep != 0x10000) {
                                entity->animStep = 0x10000;
                            }
                        }
                        if (!(Battle_ProcessActionSlot(entity) & 0xFF)) {
                            ready = 0;
                        }
                    }
                }
                entity = entity->next;
            } while (entity != 0);
        }
        if ((u8)ready != 0) {
            Akao_Cmd_21(0, 0xFF);
            Pm_StopAllBoth();
            D_8009CE74 += 1;
            return;
        }
        return;
    case 1:
        /* Finish the resource read, restore the player and reset HUD colors. */
        if (CD_StepReadState(0) != step) {
            Battle_ClearMotionTable();
            nextMode = 0x18;
            if (!(g_BattleTurnGameFlagsRead[0] & 0x1800)) {
                player = g_BattleTurnResumedPlayerView[0];
                nextMode = 0x15;
            } else {
                player = g_BattleTurnResumedPlayerView[0];
                g_BattleTurnGameFlagsWrite[0] = g_BattleTurnGameFlagsRead[0] & ~0x1800;
            }
            Entity_SetActionMode(player, nextMode);
            Battle_SetupPlayerPalette();
            c6 = 0x46;
            c5 = 0x82;
            c4 = 0x9F;
            c2 = 0xFF;
            c3 = 0xF9;
            BATTLE_HUD_BYTE(D_800B00ED) = c6;
            BATTLE_HUD_BYTE(D_800B00FD) = c6;
            BATTLE_HUD_BYTE(D_800B0111) = c6;
            BATTLE_HUD_BYTE(D_800B0121) = c6;
            c6 = 0x36;
            BATTLE_HUD_BYTE(D_800B00F4) = c4;
            BATTLE_HUD_BYTE(D_800B0104) = c4;
            BATTLE_HUD_BYTE(D_800B692C) = c4;
            BATTLE_HUD_BYTE(D_800B0118) = c4;
            BATTLE_HUD_BYTE(D_800B0128) = c4;
            BATTLE_HUD_BYTE(D_800B6948) = c4;
            c4 = 0x4A;
            BATTLE_HUD_BYTE(D_800B00F6) = c3;
            BATTLE_HUD_BYTE(D_800B0106) = c3;
            BATTLE_HUD_BYTE(D_800B692E) = c3;
            BATTLE_HUD_BYTE(D_800B011A) = c3;
            BATTLE_HUD_BYTE(D_800B012A) = c3;
            BATTLE_HUD_BYTE(D_800B694A) = c3;
            c3 = 0x3B;
            c7 = 0x3D;
            BATTLE_HUD_BYTE(D_800B00EC) = 0;
            BATTLE_HUD_BYTE(D_800B00EE) = c5;
            BATTLE_HUD_BYTE(D_800B00F5) = c2;
            BATTLE_HUD_BYTE(D_800B00FC) = 0;
            BATTLE_HUD_BYTE(D_800B00FE) = c5;
            BATTLE_HUD_BYTE(D_800B0105) = c2;
            BATTLE_HUD_BYTE(D_800B692D) = c2;
            BATTLE_HUD_BYTE(D_800B0110) = 0;
            BATTLE_HUD_BYTE(D_800B0112) = c5;
            BATTLE_HUD_BYTE(D_800B0119) = c2;
            BATTLE_HUD_BYTE(D_800B0120) = 0;
            BATTLE_HUD_BYTE(D_800B0122) = c5;
            BATTLE_HUD_BYTE(D_800B0129) = c2;
            BATTLE_HUD_BYTE(D_800B6949) = c2;
            BATTLE_HUD_BYTE(D_800B0134) = 0;
            BATTLE_HUD_BYTE(D_800B0135) = c5;
            BATTLE_HUD_BYTE(D_800B0136) = c6;
            BATTLE_HUD_BYTE(D_800B013C) = c4;
            BATTLE_HUD_BYTE(D_800B0146) = c6;
            BATTLE_HUD_BYTE(D_800B017E) = c6;
            BATTLE_HUD_BYTE(D_800B018E) = c6;
            c6 = 0x81;
            BATTLE_HUD_BYTE(D_800B0145) = c5;
            BATTLE_HUD_BYTE(D_800B017D) = c5;
            BATTLE_HUD_BYTE(D_800B018D) = c5;
            c5 = 0x83;
            BATTLE_HUD_BYTE(D_800B014C) = c4;
            BATTLE_HUD_BYTE(D_800B0184) = c4;
            BATTLE_HUD_BYTE(D_800B0194) = c4;
            c4 = 0x13;
            BATTLE_HUD_BYTE(D_800B013E) = c3;
            BATTLE_HUD_BYTE(D_800B014E) = c3;
            BATTLE_HUD_BYTE(D_800B0186) = c3;
            BATTLE_HUD_BYTE(D_800B0196) = c3;
            c3 = 1;
            BATTLE_HUD_BYTE(D_800B013D) = c2;
            BATTLE_HUD_BYTE(D_800B0144) = 0;
            BATTLE_HUD_BYTE(D_800B014D) = c2;
            BATTLE_HUD_BYTE(D_800B017C) = 0;
            BATTLE_HUD_BYTE(D_800B0185) = c2;
            BATTLE_HUD_BYTE(D_800B018C) = 0;
            BATTLE_HUD_BYTE(D_800B0195) = c2;
            BATTLE_HUD_BYTE(D_800B0158) = c2;
            BATTLE_HUD_BYTE(D_800B0159) = c7;
            BATTLE_HUD_BYTE(D_800B015A) = c6;
            BATTLE_HUD_BYTE(D_800B0160) = c5;
            BATTLE_HUD_BYTE(D_800B0161) = c4;
            BATTLE_HUD_BYTE(D_800B0162) = c3;
            BATTLE_HUD_BYTE(D_800B0168) = c2;
            BATTLE_HUD_BYTE(D_800B0169) = c7;
            BATTLE_HUD_BYTE(D_800B016A) = c6;
            BATTLE_HUD_BYTE(D_800B0170) = c5;
            BATTLE_HUD_BYTE(D_800B0171) = c4;
            BATTLE_HUD_BYTE(D_800B0172) = c3;
            BATTLE_HUD_BYTE(D_800B01A0) = c2;
            BATTLE_HUD_BYTE(D_800B01A1) = c7;
            BATTLE_HUD_BYTE(D_800B01B0) = c2;
            c2 = 0xC;
            BATTLE_HUD_BYTE(D_800B01A2) = c6;
            BATTLE_HUD_BYTE(D_800B01A8) = c5;
            BATTLE_HUD_BYTE(D_800B01A9) = c4;
            BATTLE_HUD_BYTE(D_800B01AA) = c3;
            BATTLE_HUD_BYTE(D_800B01B1) = c7;
            BATTLE_HUD_BYTE(D_800B01B2) = c6;
            BATTLE_HUD_BYTE(D_800B01B8) = c5;
            BATTLE_HUD_BYTE(D_800B01B9) = c4;
            BATTLE_HUD_BYTE(D_800B01BA) = c3;
            D_8009D28C = c2;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}

/* Finish entity death animations and restore the player for the next phase. */
extern u8 D_8009D244;
void Battle_StepEntityDeath(void)
{
    BattleEntity *entity;

    switch (D_8009CE74) {
    case 0:
        D_8009D244 = 0;
        for (entity = g_BattleTurnEntityListView[0]; entity != 0; entity = entity->next) {
            if (entity == g_BattleTurnPlayerView[0]) continue;
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else {
                u32 flags;
                Entity_SetActionMode(entity,
                    (u16)(s8)((EnemyCombatant *)entity->core)->field06.bytes.low);
                flags = entity->entityFlags;
                entity->motionX = 0;
                entity->motionY = 0;
                entity->motionZ = 0;
                entity->entityFlags = flags | 0x1000;
                asm volatile("" : : : "memory");
                if ((entity->entityFlags & 0x40000000) &&
                    !((EnemyCombatant *)entity->core)->deathPersist) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                }
            }
            Anim_SetInterpRate(&entity->renderObject, 30);
            entity->renderObject.flags_9C |= 2;
        }
        D_8009CE74++;
        break;
    case 1: {
        int all_done = 1;
        entity = g_BattleTurnEntityListView[0];
        if (entity != 0) {
            BattleEntity *player = g_BattleTurnPlayerView[0];
            do {
                if (entity == player) continue;
                if (entity->core == 0 && entity->parent == 0) continue;
                if (entity->renderObject.variant_visible == 0) {
                    entity->core = 0;
                    entity->entityFlags |= 0x10;
                } else if (entity->entityFlags & 0x40) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                } else {
                    all_done = 0;
                }
            } while ((entity = entity->next) != 0);
        }
        if ((u8)all_done) {
            Pm_StopAllBoth();
            D_8009CE74++;
        }
        break;
    }
    case 2:
        if (CD_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(g_BattleTurnPlayerView[0], 0x15);
            Battle_SetupPlayerPalette();
            D_8009D28C = 11;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}


/* Start the player and battle-HUD death sequence after death cleanup. */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK func_8006DE80();
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_b __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(u8 **)&g_PlayerEntity_o)
#define D254_B (*(u8 **)&g_PlayerEntity_b)
extern s32 g_BattleModeState;
extern s32 g_FieldMoveLock_r[] __asm__("g_FieldMoveLock");
extern s32 g_FieldMoveLock_w[] __asm__("g_FieldMoveLock");
extern struct { char _[16]; } D_800B00EC_o __asm__("D_800B00EC");
#define D_800B00EC (*(s8 *)&D_800B00EC_o)
extern struct { char _[16]; } D_800B00ED_o __asm__("D_800B00ED");
#define D_800B00ED (*(s8 *)&D_800B00ED_o)
extern struct { char _[16]; } D_800B00EE_o __asm__("D_800B00EE");
#define D_800B00EE (*(s8 *)&D_800B00EE_o)
extern struct { char _[16]; } D_800B00F4_o __asm__("D_800B00F4");
#define D_800B00F4 (*(s8 *)&D_800B00F4_o)
extern struct { char _[16]; } D_800B00F5_o __asm__("D_800B00F5");
#define D_800B00F5 (*(s8 *)&D_800B00F5_o)
extern struct { char _[16]; } D_800B00F6_o __asm__("D_800B00F6");
#define D_800B00F6 (*(s8 *)&D_800B00F6_o)
extern struct { char _[16]; } D_800B00FC_o __asm__("D_800B00FC");
#define D_800B00FC (*(s8 *)&D_800B00FC_o)
extern struct { char _[16]; } D_800B00FD_o __asm__("D_800B00FD");
#define D_800B00FD (*(s8 *)&D_800B00FD_o)
extern struct { char _[16]; } D_800B00FE_o __asm__("D_800B00FE");
#define D_800B00FE (*(s8 *)&D_800B00FE_o)
extern struct { char _[16]; } D_800B0104_o __asm__("D_800B0104");
#define D_800B0104 (*(s8 *)&D_800B0104_o)
extern struct { char _[16]; } D_800B0105_o __asm__("D_800B0105");
#define D_800B0105 (*(s8 *)&D_800B0105_o)
extern struct { char _[16]; } D_800B0106_o __asm__("D_800B0106");
#define D_800B0106 (*(s8 *)&D_800B0106_o)
extern struct { char _[16]; } D_800B0110_o __asm__("D_800B0110");
#define D_800B0110 (*(s8 *)&D_800B0110_o)
extern struct { char _[16]; } D_800B0111_o __asm__("D_800B0111");
#define D_800B0111 (*(s8 *)&D_800B0111_o)
extern struct { char _[16]; } D_800B0112_o __asm__("D_800B0112");
#define D_800B0112 (*(s8 *)&D_800B0112_o)
extern struct { char _[16]; } D_800B0118_o __asm__("D_800B0118");
#define D_800B0118 (*(s8 *)&D_800B0118_o)
extern struct { char _[16]; } D_800B0119_o __asm__("D_800B0119");
#define D_800B0119 (*(s8 *)&D_800B0119_o)
extern struct { char _[16]; } D_800B011A_o __asm__("D_800B011A");
#define D_800B011A (*(s8 *)&D_800B011A_o)
extern struct { char _[16]; } D_800B0120_o __asm__("D_800B0120");
#define D_800B0120 (*(s8 *)&D_800B0120_o)
extern struct { char _[16]; } D_800B0121_o __asm__("D_800B0121");
#define D_800B0121 (*(s8 *)&D_800B0121_o)
extern struct { char _[16]; } D_800B0122_o __asm__("D_800B0122");
#define D_800B0122 (*(s8 *)&D_800B0122_o)
extern struct { char _[16]; } D_800B0128_o __asm__("D_800B0128");
#define D_800B0128 (*(s8 *)&D_800B0128_o)
extern struct { char _[16]; } D_800B0129_o __asm__("D_800B0129");
#define D_800B0129 (*(s8 *)&D_800B0129_o)
extern struct { char _[16]; } D_800B012A_o __asm__("D_800B012A");
#define D_800B012A (*(s8 *)&D_800B012A_o)
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
extern s32 D_800B0CD8_r[] __asm__("g_GameState");
extern s32 D_800B0CD8_w[] __asm__("g_GameState");
extern struct { char _[16]; } D_800B692C_o __asm__("D_800B692C");
#define D_800B692C (*(s8 *)&D_800B692C_o)
extern struct { char _[16]; } D_800B692D_o __asm__("D_800B692D");
#define D_800B692D (*(s8 *)&D_800B692D_o)
extern struct { char _[16]; } D_800B692E_o __asm__("D_800B692E");
#define D_800B692E (*(s8 *)&D_800B692E_o)
extern struct { char _[16]; } D_800B6948_o __asm__("D_800B6948");
#define D_800B6948 (*(s8 *)&D_800B6948_o)
extern struct { char _[16]; } D_800B6949_o __asm__("D_800B6949");
#define D_800B6949 (*(s8 *)&D_800B6949_o)
extern struct { char _[16]; } D_800B694A_o __asm__("D_800B694A");
#define D_800B694A (*(s8 *)&D_800B694A_o)

void Battle_StartDeathAnim(void) {
    s32 c2;
    s32 c3;
    s32 c4;
    s32 c5;
    s32 c6;
    register s32 c7 asm("$7");
    u8 *p254;
    s32 t98;

    Battle_ResetEnemyStats(0);
    c6 = 0x46;
    c5 = 0x82;
    c4 = 0x9F;
    c2 = 0xFF;
    c3 = 0xF9;
    D_800B00ED = c6;
    D_800B00FD = c6;
    D_800B0111 = c6;
    D_800B0121 = c6;
    c6 = 0x36;
    D_800B00F4 = c4;
    D_800B0104 = c4;
    D_800B692C = c4;
    D_800B0118 = c4;
    D_800B0128 = c4;
    D_800B6948 = c4;
    c4 = 0x4A;
    D_800B00F6 = c3;
    D_800B0106 = c3;
    D_800B692E = c3;
    D_800B011A = c3;
    D_800B012A = c3;
    D_800B694A = c3;
    c3 = 0x3B;
    c7 = 0x3D;
    D_800B00EC = 0;
    D_800B00EE = c5;
    D_800B00F5 = c2;
    D_800B00FC = 0;
    D_800B00FE = c5;
    D_800B0105 = c2;
    D_800B692D = c2;
    D_800B0110 = 0;
    D_800B0112 = c5;
    D_800B0119 = c2;
    D_800B0120 = 0;
    D_800B0122 = c5;
    D_800B0129 = c2;
    D_800B6949 = c2;
    D_800B0134 = 0;
    D_800B0135 = c5;
    D_800B0136 = c6;
    D_800B013C = c4;
    D_800B0146 = c6;
    D_800B017E = c6;
    D_800B018E = c6;
    c6 = 0x81;
    D_800B0145 = c5;
    D_800B017D = c5;
    D_800B018D = c5;
    c5 = 0x83;
    D_800B014C = c4;
    D_800B0184 = c4;
    D_800B0194 = c4;
    c4 = 0x13;
    D_800B013E = c3;
    D_800B014E = c3;
    D_800B0186 = c3;
    D_800B0196 = c3;
    c3 = 1;
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
    D_800B01B0 = c2;
    c2 = 2;
    g_BattleModeState = c2;
    __asm__ __volatile__("");
    {
        s32 d2e8;
        d2e8 = g_FieldMoveLock_r[0];
                D_800B01A8 = c5;
        D_800B01B8 = c5;
        p254 = g_PlayerEntity;
        __asm__ __volatile__("" :: "r"(p254));
        D_800B01AA = c3;
        D_800B01BA = c3;
        c3 = ~0x100;
                D_800B01A2 = c6;
        D_800B01A9 = c4;
        D_800B01B1 = c7;
        D_800B01B2 = c6;
        D_800B01B9 = c4;
                g_FieldMoveLock_w[0] = d2e8 | 1;
    }
    t98 = *(s32 *) (p254 + 0x98);
    *(s32 *) (p254 + 0x68) = 0;
    *(s32 *) (p254 + 0x6C) = 0;
    *(s32 *) (p254 + 0x70) = 0;
    *(s32 *) (p254 + 0x98) = t98 & c3;
    D_800B0CD8_r[0] = D_800B0CD8_r[0] | 0x8000;
    if (!(g_GameStateFlags & 0x1800)) {
        Entity_SetActionMode(p254, 0x14);
        {
            u8 *q;
            q = D254_B;
            func_8006DE80(0x45B, 0, *(s16 *) (q + 0x2A), *(s16 *) (q + 0x2E), (s32) *(s16 *) (q + 0x32));
        }
    }
}

#undef BATTLE_HUD_SLOT_BYTE
#undef BATTLE_HUD_BYTE
