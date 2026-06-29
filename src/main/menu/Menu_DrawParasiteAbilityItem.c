extern int D_800A18EC[];

void Menu_UpdateStatBarAnimation(int);
void Stat_QueryLevelAndSubLevel(int, int, int *, int);
void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);
void Draw_PrintNumberWidth2(int);

void Menu_DrawParasiteAbilityItem(int arg0) {
    int value;
    int local;

    value = D_800A18EC[arg0];

    Draw_OffsetCursor(2, 1);
    Draw_AllocSprite(arg0 + 0x91);
    Draw_OffsetCursor(0x42, -1);

    arg0 += 5;
    Stat_QueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
    Draw_OffsetCursor(2, 0);
    Menu_UpdateStatBarAnimation(arg0);
    Draw_OffsetCursor(-0x44, 0xE);
}
