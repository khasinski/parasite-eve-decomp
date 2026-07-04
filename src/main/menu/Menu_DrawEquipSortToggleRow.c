extern short D_800922D0[];

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_AllocSprite(int arg0);


void Menu_DrawEquipSortToggleRow(int arg0) {
    short *p;

    Draw_OffsetCursor(0, 2);
    Draw_AllocSprite(D_800922D0[arg0]);
    Draw_OffsetCursor(0x12, 4);
    Draw_AllocSprite(0x22);
    Draw_OffsetCursor(0x0C, -4);

    p = D_800922D0;
    if (arg0 == 0) {
        p++;
    }
    Draw_AllocSprite(*p);
}
