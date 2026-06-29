/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuInputPausePending;
extern unsigned char g_DiscChangeFlags[];

void MenuInput_SetPollingPaused(int arg0);

void Evt_DeferredExec(void) {
    if (g_MenuInputPausePending != 0) {
        if ((g_DiscChangeFlags[0] & 3) == 0) {
            MenuInput_SetPollingPaused(0);
            g_MenuInputPausePending = 0;
        }
    }
}
