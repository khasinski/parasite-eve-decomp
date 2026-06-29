/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuBattleEquipMode;

void Menu_SetBattleEquipMode(int value) {
    g_MenuBattleEquipMode = value;
}
