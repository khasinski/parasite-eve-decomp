void Inv_ShowItemDescriptionPanel(void);

void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);

void MenuWidget_DrawCenteredTableText(int arg0);

void Menu_DrawArmorEquipItem(void) {
    Inv_ShowItemDescriptionPanel();
}

void Menu_DrawMemCardPortItem(int arg0) {
    Draw_OffsetCursor(-2, -2);
    if (arg0 < 2) {
        Draw_AllocSprite(arg0 + 0x84);
    } else {
        Draw_AllocSprite(0x62);
    }
}

void Menu_DrawMainMenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x28);
}
