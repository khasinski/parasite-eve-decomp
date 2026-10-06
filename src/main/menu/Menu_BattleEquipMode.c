/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuBattleEquipMode;

void Menu_SetBattleEquipMode(int value) {
    g_MenuBattleEquipMode = value;
}

int Menu_GetBattleEquipMode(void) {
    return g_MenuBattleEquipMode;
}
