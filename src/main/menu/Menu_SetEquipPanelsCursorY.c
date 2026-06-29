void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_SetCursorY(void *node);

void Menu_SetEquipPanelsCursorY(void) {
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}
