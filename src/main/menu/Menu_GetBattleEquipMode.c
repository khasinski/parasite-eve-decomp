/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuBattleEquipMode;

int Menu_GetBattleEquipMode(void) {
    return g_MenuBattleEquipMode;
}
