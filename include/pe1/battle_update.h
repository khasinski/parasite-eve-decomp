#ifndef PE1_BATTLE_UPDATE_H
#define PE1_BATTLE_UPDATE_H

#include "pe1/battle_turn.h"

int Akao_SendTableCommand(void *, int, int, int, int);
int Asset_Find08Alt(int, int, int, int, int);
int Battle_BuildTargetList(void);
void Battle_DrawActiveStatus(void);
int Battle_HandleItemMenu(void);
void Battle_Init(void);
void Battle_PhaseHitReaction(void);
void Battle_ResolveHitOnTimer(void);
void Battle_RollEnemySpawn(int);
void Battle_StepEntityDeath(void);
void Battle_StepEscapeOrDeath(void);
void Battle_StepLevelUp(void);
void Battle_StepPostBattle(void);
void Battle_StepScriptEntry(void);
int Battle_StepVictory(void);
void Gpu_QueuePrimitive(void);
int Inv_CountByValue(int);
int Inv_FindItemById(int);
void Inventory_OpenAyaItemList(unsigned int);
void Menu_MainUpdate(int);
int Menu_RunFrameWithArg(int);
void Menu_SaveOverlayDraw(void);
void Menu_SetItemContext(int);
int Pad_GetMenuPressedBitOrDisabled(void);
int Pm_SendCmd(int, int, int, int, int, int);
void Render_BeginSceneLoad(void);
void Save_DrawSlotMetadata(void);
void srand(unsigned int);
/* Signed retail LB; battle_palette.h also exposes an unsigned view. */
extern s8 g_BattleUpdatePaletteEnabled asm("D_8009D2A0");

/* Only element zero is accessed in the wide matching views below. */
extern u8 D_8009CE7C;
extern s32 D_8009D1AC[16];
extern u8 D_8009D1CE[16];
extern s32 D_8009D1D0;
extern s32 D_8009D1E8;
extern s8 D_8009D1F0[16];
extern s32 D_8009D1F4[16];
extern s32 D_8009D230[16];
extern u8 D_8009D235[16];
extern u8 D_8009D23C;
extern u8 D_8009D244;
extern u8 D_8009D288;
extern s32 D_8009D290[16];
extern u8 D_8009D294[16];
extern s16 D_8009D2A4;
extern s8 D_8009D2B0;
extern s32 D_8009D2FC;
extern BattleAction D_800A76D8;
extern void *D_800B0E08[16];
extern u8 D_800B8A90[16];

#endif
