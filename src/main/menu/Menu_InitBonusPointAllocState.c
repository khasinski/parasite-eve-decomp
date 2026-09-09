/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/aya.h"

typedef short s16;

extern int g_MenuPendingTotalExp;
extern int g_MenuExpAllocTarget;
extern int g_MenuLevelDisplayValue;
extern int D_8009CFF0;
extern int g_MenuLevelDisplayTarget;
extern int g_MenuHpMaxDisplayValue;
extern int g_MenuHpMaxDisplayTarget;
extern int g_BonusPointDisplayValue;
extern int g_MenuBonusPointDisplayTarget;
extern int g_BonusPointBarAnimStep;
extern int g_BonusPointStatQueryResults[];
extern int g_BonusPointStatDeltas[];
extern int g_BonusPointStatMultipliers[];
extern AyaSaveState D_800C0E00;

void BattleCmd_SyncActiveAmmo(void);
void Stat_QueryLevelAndSubLevel(int stat, int value, int *out, int arg3);
void Inv_LoadWayneItemsAsOverride(int arg0);
void Menu_CreateLevelUpResultPanel(void);
void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_OffsetPosition(void *node, int x, int y);
void Akao_SetBgmVolumeFade(void);
void Menu_ResetInputState(void);

void Menu_InitBonusPointAllocState(int gained_points) {
    int scratch[2];
    s16 *source;
    int *deltas;
    int *query_results;
    int *multipliers;
    int stat;
    int value;

    BattleCmd_SyncActiveAmmo();

    g_MenuPendingTotalExp = D_800C0E00.total_exp;
    g_MenuExpAllocTarget = D_800C0E00.total_exp;
    g_MenuLevelDisplayValue = D_800C0E00.level;
    D_8009CFF0 = D_800C0E00.level;
    g_MenuLevelDisplayTarget = D_800C0E00.level;
    g_MenuHpMaxDisplayValue = D_800C0E00.max_hp;
    g_MenuHpMaxDisplayTarget = D_800C0E00.max_hp;
    g_BonusPointDisplayValue = D_800C0E00.bonus_points;

    if (gained_points < 0) {
        gained_points = 0;
    }
    g_MenuBonusPointDisplayTarget = D_800C0E00.bonus_points + gained_points;

    source = (s16 *)&D_800C0E00.stat_agility;
    stat = 0;
    multipliers = g_BonusPointStatMultipliers;
    query_results = g_BonusPointStatQueryResults;
    deltas = g_BonusPointStatDeltas;
    do {
        value = *source++;
        *deltas = value;
        Stat_QueryLevelAndSubLevel(stat, value, query_results, 0);
        *multipliers = 0;
        multipliers++;
        query_results++;
        deltas++;
        stat++;
    } while (stat < 7);

    Stat_QueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0], scratch, 0);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(0);
    Menu_CreateLevelUpResultPanel();
    MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x15), 0, 0x20);
    if (gained_points > 0) {
        Akao_SetBgmVolumeFade();
    }
    Menu_ResetInputState();
}

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_SetCurrentNode();
void Menu_InitStateTables(void);
void *MenuWidget_CreateSimpleNode(s32 mode, s32 arg1, s32 arg2, s32 arg3);
void *MenuWidget_CreateNode(s32 mode, void *parent, void *sibling);

extern s32 g_MenuBonusPointBarAnimActive;
extern struct { char _[16]; } D_80092314_o __asm__("D_80092314");
#define D_80092314 (*(void *)&D_80092314_o)
extern struct { char _[16]; } func_8004BF40_o __asm__("Menu_DrawBonusPointAnimFrame");
#define Menu_DrawBonusPointAnimFrame (*(void *)&func_8004BF40_o)
extern struct { char _[16]; } func_8004C1E0_o __asm__("Menu_StepParasiteScreen");
#define Menu_StepParasiteScreen (*(void *)&func_8004C1E0_o)
extern struct { char _[16]; } func_8004FFF8_o __asm__("Menu_DrawStatAllocationList");
#define Menu_DrawStatAllocationList (*(void *)&func_8004FFF8_o)

void Menu_CreateLevelUpResultPanel(void) {
    s32 current_y;
    register s32 pos asm("$3");
    register void *callback asm("$3");
    void *root;
    void *child;
    register void *current asm("$4");

    root = MenuWidget_CreateSimpleNode(0x15, 0, 0, 0);
    child = MenuWidget_CreateNode(0x2F, root, root);
    current = root;
    callback = &Menu_DrawBonusPointAnimFrame;
    M2C_FIELD(root, void **, 0x30) = callback;
    callback = &Menu_StepParasiteScreen;
    M2C_FIELD(root, void **, 0x2C) = callback;
    callback = &Menu_DrawStatAllocationList;
    M2C_FIELD(child, void **, 0x30) = callback;
    pos = M2C_FIELD(child, s32 *, 0x18);
    current_y = -1;
    M2C_FIELD(child, s32 *, 0x44) = current_y;
    current_y = M2C_FIELD(child, s32 *, 0x40);
    pos += 0x44;
    M2C_FIELD(child, s32 *, 0x18) = pos;
    pos = M2C_FIELD(child, s32 *, 0x1C);
    current_y -= 2;
    M2C_FIELD(child, s32 *, 0x40) = current_y;
    pos += 2;
    M2C_FIELD(child, s32 *, 0x1C) = pos;
    MenuWidget_SetCurrentNode(current, current_y);
    M2C_FIELD(root, void **, 0x4C) = &D_80092314;
    g_MenuBonusPointBarAnimActive = 1;
    Menu_InitStateTables();
}


extern int g_MenuStatBarWidths[8];
extern int g_MenuStatBarSettleTimers[8];

void Menu_InitStateTables(void) {
    int i;

    for (i = 0; i < 8; i++) {
        g_MenuStatBarSettleTimers[i] = 0;
        g_MenuStatBarWidths[i] = 0;
    }
}

#undef Menu_DrawBonusPointAnimFrame

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Aya_UnlockParasiteSpellById();
M2C_UNK Draw_OffsetCursor();
M2C_UNK Draw_PrintNumberWidth5();
M2C_UNK Draw_SetColor();
M2C_UNK Menu_UpdateStatBarAnimation();
M2C_UNK Akao_FlushBgmVolumeFade();
M2C_UNK Draw_AllocSprite();
extern s32 g_BonusPointBarAnimProgress;
extern s32 g_MenuLevelDisplayValue;
extern s32 g_MenuHpMaxDisplayValue;
extern s32 g_BonusPointDisplayValue;
extern s32 g_MenuLevelDisplayTarget;
extern s32 g_MenuHpMaxDisplayTarget;
extern s32 g_MenuBonusPointDisplayTarget;
extern s32 g_MenuLevelUpAnimTimer;
extern s32 D_8009CF7C;
extern s32 g_MenuBonusPointBarAnimActive;
extern s32 g_BonusPointBarAnimStep;
extern u16 g_AyaHpMax[];
#define g_AyaHpMax (g_AyaHpMax[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])
extern u8 g_AyaSaveLevel[];
#define g_AyaSaveLevel (g_AyaSaveLevel[0])

void Menu_DrawBonusPointAnimFrame(void) {
    M2C_UNK var_a0;
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_s0;
    s32 var_s0_2;

    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(0x97);
    var_s0 = 0;
    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        temp_v0 = g_MenuLevelUpAnimTimer < 0x1E;
        g_MenuLevelUpAnimTimer += 1;
        var_s0 = 1;
        if (temp_v0 == 0) {
            temp_a0 = g_MenuLevelDisplayValue + 1;
            g_MenuLevelUpAnimTimer = 0;
            g_MenuLevelDisplayValue = temp_a0;
            Aya_UnlockParasiteSpellById(temp_a0);
        }
    }
    var_a0 = 0x808080;
    if ((s32) g_AyaSaveLevel < g_MenuLevelDisplayValue) {
        var_a0 = 0x8080;
    }
    Draw_SetColor(var_a0);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuLevelDisplayValue + 1);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(-0x3C, 0x14);
    Draw_AllocSprite(0x99);
    if (g_MenuHpMaxDisplayValue < g_MenuHpMaxDisplayTarget) {
        temp_v1 = D_8009CF7C;
        D_8009CF7C = temp_v1 + 1;
        var_s0 = 1;
        if (temp_v1 >= 0) {
            D_8009CF7C = 0;
            g_MenuHpMaxDisplayValue += 1;
        }
    }
    var_a0_2 = 0x808080;
    if ((s32) g_AyaHpMax < g_MenuHpMaxDisplayValue) {
        var_a0_2 = 0x8080;
    }
    Draw_SetColor(var_a0_2);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuHpMaxDisplayValue);
    Draw_OffsetCursor(-0x3C, 0x13);
    Draw_SetColor(0x808080);
    Draw_AllocSprite(0x93);
    if (g_BonusPointDisplayValue < g_MenuBonusPointDisplayTarget) {
        g_BonusPointDisplayValue += 1;
        var_s0 = 1;
    }
    temp_v1 = g_BonusPointDisplayValue;
    var_a0_3 = 0x808080;
    if (g_AyaBonusPoints < temp_v1) {
        var_a0_3 = 0x8080;
    }
    Draw_SetColor(var_a0_3);
    Draw_OffsetCursor(0xF, 0x11);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
    Draw_OffsetCursor(0xA, -0x56);
    Draw_SetColor(0x808080);
    if (g_MenuBonusPointBarAnimActive != 0) {
        if (g_BonusPointBarAnimProgress < 0x80) {
            g_BonusPointBarAnimProgress += g_BonusPointBarAnimStep;
        }
        if ((var_s0 == 0) && (g_BonusPointBarAnimProgress >= 0x80)) {
            g_BonusPointBarAnimProgress = 0x80;
            g_MenuBonusPointBarAnimActive = 0;
        }
    }
    Draw_OffsetCursor(0x78, 2);
    var_s0_2 = 1;
    do {
        Menu_UpdateStatBarAnimation(var_s0_2);
        Draw_OffsetCursor(0, 0x10);
        var_s0_2 += 1;
    } while (var_s0_2 < 7);
    if (g_MenuBonusPointBarAnimActive == 0) {
        Akao_FlushBgmVolumeFade();
    }
}
