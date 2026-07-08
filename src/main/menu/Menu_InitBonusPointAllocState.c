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
