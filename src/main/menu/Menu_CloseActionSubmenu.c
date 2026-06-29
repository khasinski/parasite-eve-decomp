void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_DestroyNode(void *node);
void MenuWidget_RestoreSavedCurrentNode(void);

void Menu_CloseActionSubmenu(int arg0) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, arg0 + 0x29));

    if (arg0 != 0) {
        MenuWidget_RestoreSavedCurrentNode();
    }
}
