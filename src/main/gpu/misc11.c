/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int g_MenuInputActive;

int Draw_RemapStatusFlags(void);

int g_DrawPresentEnabled;

int MenuInput_GetStatusFlags(void) {
    if (g_MenuInputActive != 0) {
        return Draw_RemapStatusFlags();
    }
    return 0;
}

void Draw_SetPresentEnabled(int arg0) {
    g_DrawPresentEnabled = arg0;
}
