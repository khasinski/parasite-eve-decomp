/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuCommandResult;

void Menu_ClearCommandResult(void) {
    g_MenuCommandResult = 0;
}
