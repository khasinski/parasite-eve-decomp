void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);

void Menu_DrawMemCardPortItem(int arg0) {
    Draw_OffsetCursor(-2, -2);
    if (arg0 < 2) {
        Draw_AllocSprite(arg0 + 0x84);
    } else {
        Draw_AllocSprite(0x62);
    }
}
