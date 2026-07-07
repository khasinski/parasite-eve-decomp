/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int *g_ActiveActor[];
signed char g_BattleActionQueueCount;

int Pad_GetMenuPressedBitOrDisabled(void) {
    if (g_ActiveActor[0][0x13] & 0x10000) {
        return -1;
    }
    return g_BattleActionQueueCount;
}
