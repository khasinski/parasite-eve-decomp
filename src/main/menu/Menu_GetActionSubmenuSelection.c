/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern unsigned int g_MenuActionSubmenuSel;

unsigned int Menu_GetActionSubmenuSelection(void) {
    return g_MenuActionSubmenuSel;
}
