/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

extern int g_MenuStatBarWidths[8];
extern int g_MenuStatBarSettleTimers[8];

void Menu_InitStateTables(void) {
    int i;

    for (i = 0; i < 8; i++) {
        g_MenuStatBarSettleTimers[i] = 0;
        g_MenuStatBarWidths[i] = 0;
    }
}
