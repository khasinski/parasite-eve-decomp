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
int Menu_RunFrameWithArg(void *arg);
int Pad_GetMenuPressedBitOrDisabled(void);
void Battle_StepScriptEntry(void);
void Battle_ResolveHitOnTimer(void);
void Save_DrawSlotMetadata(void);
void Menu_MainUpdate(int mode);
void Battle_UpdateEnemy(void *entity);
void Pm_SendCmd(int slot, int arg1, int arg2, void *arg3, int arg4, int arg5);
void Gpu_QueuePrimitive(void);
int Battle_RollEnemySpawn(int arg0);
int Battle_HandleItemMenu(void);
void Battle_StepLevelUp(void);
int Battle_StepVictory(void);
int Inv_CountByValue(int item_id);
int Inv_FindItemById(int item_id);
void Battle_StepPostBattle(void);
void Battle_DrawStatusPanel(int side, void *panel);
void Battle_StepEscapeOrDeath(void);
void Battle_StepEntityDeath(void);
void Battle_PhaseInitEnemyTurn(void);
void Battle_PhaseHitReaction(void);
void Battle_PhaseEndTurn(void);
void Menu_SaveOverlayDraw(void);
void Battle_DrawActiveStatus(void);

extern u8 D_8009D2EC;
extern u8 D_8009D23C;
extern u32 D_8009D230;
extern u8 D_8009D294;
extern u32 D_8009D1F4;
extern int D_8009D2FC;
extern char D_800A76D8[];
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

static void Battle_UpdateSuspendState(void) {
    Combatant *actor;

    actor = D_8009D278;
    if ((actor->stateFlags & 0x80000) != 0) {
        if (D_8009D28C == 6) {
            D_8009CE7C = 6;
            D_8009D28C = 0;
        }
    } else if (D_8009CE7C == 6) {
        D_8009CE7C = 0;
        D_8009D28C = 6;
    }
}

static void Battle_UpdateEnemies(void) {
    BattleEntity *entity;

    entity = g_FieldActorListHead;
    while (entity != 0) {
        if (entity != (BattleEntity *)D_8009D254 && entity->core != 0) {
            Battle_UpdateEnemy(entity);
        }
        entity = entity->next;
    }
}

static void Battle_UpdateTimedPanels(void) {
    Combatant *actor;

    actor = D_8009D278;
    if (actor->panelA_timer != 0) {
        Battle_DrawStatusPanel(0, &actor->panelA_val);
        actor->panelA_timer--;
    }
    if (actor->field66 != 0) {
        Battle_DrawStatusPanel(0, &actor->field66 - 6);
        actor->field66--;
    }
}

static void Battle_UpdateQueuedPmCommands(void) {
    if (D_8009D23C != 0) {
        Pm_SendCmd(D_8009D2FC, 0, 1, D_800B8A90, 0, 0);
        Pm_SendCmd(D_8009D2FC, 0, 0, (void *)1, 0, 0);
        D_8009D23C = 0;
    }
}

static void Battle_UpdatePlayerAnimRecovery(void) {
    BattleEntity *entity;
    Combatant *actor;

    entity = (BattleEntity *)D_8009D254;
    actor = D_8009D278;

    if (entity->animFrame != entity->animFrameMax) {
        return;
    }

    if (entity->animState < 4) {
        if (D_8009D298[0] == 0) {
            Entity_SetActionMode(entity, actor->actionMode12);
            if ((D_800B0CE6_PALETTE & 1) == 0 &&
                Pad_GetMenuPressedBitOrDisabled() == 0 &&
                D_8009D28C == 0 &&
                actor->action != 0 &&
                (actor->action->turnWord & 0x8000) != 0 &&
                D_8009D1D0 != 0) {
                Battle_RollEnemySpawn(D_8009D1D0);
                D_8009D1D0 = 0;
            }
        } else {
            if (D_8009D298[0] >= 2) {
                entity->entityFlags |= 0x100;
            }
            D_8009D298[0] = 0;
            Entity_SetActionMode(entity, D_8009D29A[0]);
            entity->pad_010[4] = (u8)(D_8009D29C[0] >> 24);
        }
    }

    if (entity->animState == 0x0D && (D_800B0CE6_PALETTE & 1) == 0) {
        actor->actionMode12 = 4;
        if ((entity->entityFlags & 0x100) != 0) {
            Asset_Find08Alt(0x453, 0, entity->worldX, entity->worldY, entity->worldZ);
            entity->entityFlags &= ~0x100;
        }
        if (entity->animFrame == entity->animFrameMax) {
            Entity_SetActionMode(entity, actor->actionMode12);
            D_8009D2E8 &= ~1;
        }
    }
}

static void Battle_UpdateNormalPhase(int menu_mode) {
    int pressed;
    Combatant *actor;

    actor = D_8009D278;

    if (D_8009D244 != 0 && actor->hpAlive >= 0x2328) {
        actor->hpAlive = 0x2328;
        if (D_8009D288 == 0) {
            srand(D_8009D250);
            D_8009D288 = 1;
        }
    }

    pressed = Pad_GetMenuPressedBitOrDisabled();
    if (pressed > 0) {
        if ((actor->stateFlags & 0x4000) == 0 || (D_8009D1A0 & 0x100) != 0) {
            Battle_StepScriptEntry();
        }
        if (D_8009D294 != 0) {
            Battle_ResolveHitOnTimer();
        }
    }

    if ((D_8009D1AC & 0x300) != 0) {
        Save_DrawSlotMetadata();
    }

    Menu_MainUpdate(menu_mode);
    Battle_UpdateEnemies();
    Battle_UpdateQueuedPmCommands();

    if (D_8009D235 != 0) {
        Gpu_QueuePrimitive();
    }

    Battle_UpdatePlayerAnimRecovery();
    D_8009D1E8++;
}

static void Battle_UpdateScriptedPhase(void) {
    switch (D_8009D28C) {
    case 1:
        D_8009D28C = (s8)Battle_HandleItemMenu();
        if (D_8009D28C == 0) {
            D_8009D1A0 &= ~4;
        }
        break;
    case 2:
        Battle_StepLevelUp();
        break;
    case 3:
        if ((D_8009D278->stateFlags & 0x800) != 0) {
            if (Battle_StepVictory() != 0) {
                D_8009D278->stateFlags &= ~0x800;
            }
        } else if (Inv_CountByValue(0x12) != 0) {
            if (Battle_StepVictory() != 0) {
                Inv_FindItemById(0x12);
            }
        } else {
            Battle_StepPostBattle();
        }
        Battle_UpdateTimedPanels();
        Battle_UpdateEnemies();
        break;
    case 4:
        Battle_StepEscapeOrDeath();
        break;
    case 5:
        Battle_StepEntityDeath();
        break;
    case 6:
        Battle_PhaseInitEnemyTurn();
        break;
    case 7:
        Battle_PhaseHitReaction();
        break;
    case 8:
        Battle_PhaseEndTurn();
        break;
    default:
        break;
    }
}

void Battle_Update(void) {
    int menu_result;
    BattleEntity *player;

    Battle_UpdateSuspendState();

    player = (BattleEntity *)D_8009D254;
    D_8009D278 = player->core;
    D_8009D230 = 0;

    menu_result = Menu_RunFrameWithArg(D_800A76D8);
    D_8009D2A4 = menu_result;

    if (D_8009D28C == 0 && D_8009D244 != 0) {
        Battle_UpdateNormalPhase(1);
    } else {
        Battle_UpdateScriptedPhase();
    }

    if (D_8009D1CE != 0 && D_8009D28C == 0) {
        Menu_SaveOverlayDraw();
    }

    if (D_8009D244 != 0) {
        Battle_DrawActiveStatus();
    }

    if (D_8009D2A4 != 0) {
        Render_BeginSceneLoad();
    }
}

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
