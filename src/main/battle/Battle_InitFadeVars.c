/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern signed char g_BattleTargetCount[];
unsigned char g_BattleFadeLevel;
signed char D_8009CE6C;

void Battle_InitFadeVars(void) {
    if (g_BattleTargetCount[0] != 0) {
        g_BattleFadeLevel = 0x80;
        D_8009CE6C = -8;
    }
}
