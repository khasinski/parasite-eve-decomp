/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuBattleCount;
short g_BattleCountTable[];

int Menu_GetBattleCountEntry(int arg0) {
    if (arg0 >= 0 && arg0 < g_MenuBattleCount) {
        return g_BattleCountTable[arg0];
    }
    return 0;
}
