void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void MenuWidget_DestroyNode(void *arg0);

void Menu_CloseContextHelpPanel(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
}
