void MenuWidget_DrawCenteredTableText(int arg0);
void Draw_DropShadow(void);
int MenuWidget_GetColumnLayoutMode(void);

void Menu_DrawBlendOptionItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x31);

    if (MenuWidget_GetColumnLayoutMode() == arg0) {
        Draw_DropShadow();
    }
}
