/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int g_MenuInputActive;

int Draw_RemapStatusFlags(void);

int MenuInput_GetStatusFlags(void) {
    if (g_MenuInputActive != 0) {
        return Draw_RemapStatusFlags();
    }
    return 0;
}
