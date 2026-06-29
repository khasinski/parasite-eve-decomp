/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuItemContextFlag;

int Menu_GetItemContextFlag(void) {
    return g_MenuItemContextFlag;
}
