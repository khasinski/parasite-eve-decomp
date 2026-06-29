/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuBattleSelectedItem;

int Menu_GetBattleEquipMode(void);
void MenuWidget_NavScrollTo(int arg0);
void Battle_UseItem(int arg0);

void Menu_OnBattleCommandConfirm(int arg0, int arg1) {
    if (arg1 != 0) {
        if (Menu_GetBattleEquipMode() != 0) {
            MenuWidget_NavScrollTo(8);
        }
        Battle_UseItem(g_MenuBattleSelectedItem);
    }
}
