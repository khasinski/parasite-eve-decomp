/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuBattleCount;

int Menu_GetBattleCount(void) {
    return g_MenuBattleCount;
}
