
void Menu_DrawBattleCommandItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawBattleCommandList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBattleCommandItem);
}
