/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_MenuInputActive;

int Draw_RemapStatusFlags(void);

int MenuInput_HasNavigationRepeat(void) {
    int ret;
    int mask;

    ret = 0;
    if (g_MenuInputActive != 0) {
        mask = Draw_RemapStatusFlags() & 0x5000;
        ret = mask != 0;
    }
    return ret;
}
