/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuCommandResult;

int Menu_GetCommandResult(void) {
    return g_MenuCommandResult;
}
