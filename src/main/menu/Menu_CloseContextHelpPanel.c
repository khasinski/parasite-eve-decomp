/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_DestroyNode(void *node);

void Menu_CloseContextHelpPanel(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
}
