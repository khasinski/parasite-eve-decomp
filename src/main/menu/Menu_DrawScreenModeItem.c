
extern int g_StatBaseTable[];

void Draw_OffsetCursor(int, int);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_PrintTextById(int);
void Draw_PrintNumberWidth2Unk(int);
void Draw_PrintNumberWidth3Unk(int);

void Menu_DrawScreenModeItem(int arg0) {
    Draw_StatePush();
    Draw_PrintTextById((arg0 & 1) | 0x6A);

    if ((arg0 >= 2) || (g_StatBaseTable[arg0] >= 100)) {
        Draw_OffsetCursor(0x28, 3);
        Draw_PrintNumberWidth3Unk(g_StatBaseTable[arg0]);
    } else {
        Draw_OffsetCursor(0x2D, 3);
        Draw_PrintNumberWidth2Unk(g_StatBaseTable[arg0]);
    }

    Draw_StatePop();
}
