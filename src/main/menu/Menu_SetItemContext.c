/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuItemSelectFlag;

void Menu_SetItemContext(int arg0) {
    g_MenuItemSelectFlag = arg0;
}
