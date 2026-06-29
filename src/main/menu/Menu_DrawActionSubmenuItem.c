void MenuWidget_DrawCenteredTableText(int arg0);
void Draw_DropShadow(void);
int Menu_GetActionSubmenuSelection(void);

void Menu_DrawActionSubmenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x2E);

    if (Menu_GetActionSubmenuSelection() == arg0) {
        Draw_DropShadow();
    }
}
