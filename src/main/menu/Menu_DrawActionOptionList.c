void Menu_DrawActionOptionRow(void);
void MenuWidget_DrawList(void *arg0, void (*arg1)(void));

void Menu_DrawActionOptionList(void *arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawActionOptionRow);
}
