void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void Menu_DrawContextHelpText(void);

void Menu_CreateContextHelpPanel(void) {
    void *node = MenuWidget_FindByModeAndSelectedBase(1, 0x13);

    if (node == 0) {
        *(void **)((char *)MenuWidget_CreateSimpleNode(0x13, 0, 0, 0) + 0x30) = Menu_DrawContextHelpText;
    }
}
