extern char g_AyaBattleState[];
extern char g_SavedBattleStateTail[];
extern int g_BattleEquipStateBlock;
extern void *D_800B8A88;
extern void *D_800B8A8C;

void Inv_RecalcSlotStats(void);
void Inv_BuildWeaponList(int arg0, void *arg1);
void Inv_BuildArmorList(void *arg0);

void Battle_InitEquipLists(void **arg0) {
    *arg0 = g_AyaBattleState;
    D_800B8A88 = g_SavedBattleStateTail;
    D_800B8A8C = &g_BattleEquipStateBlock;
    Inv_RecalcSlotStats();
    Inv_BuildWeaponList(0, D_800B8A88);
    Inv_BuildArmorList(D_800B8A8C);
}
