void Menu_DrawScreenModeItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawScreenModeList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawScreenModeItem);
}
