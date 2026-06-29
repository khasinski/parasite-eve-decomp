/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuWidgetColumnLayoutMode;

int MenuWidget_GetColumnLayoutMode(void) {
    return g_MenuWidgetColumnLayoutMode;
}
