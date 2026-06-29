/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuItemContextFlag;

int Menu_ClampRange(int arg0) {
    arg0 &= 0x1FF;
    g_MenuItemContextFlag = arg0;
    if (arg0 == 0) {
        g_MenuItemContextFlag = 0x1FF;
    }
    return 0;
}
