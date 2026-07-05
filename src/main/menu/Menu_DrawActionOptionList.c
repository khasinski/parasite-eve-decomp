void Menu_DrawActionOptionRow(int arg0);
void MenuWidget_DrawList(void *arg0, void (*arg1)(void));

void Menu_DrawActionOptionList(void *arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawActionOptionRow);
}
