void Draw_OffsetCursor(int arg0, int arg1);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_SetColor(int arg0);
void Draw_PrintTextById(int arg0);
void Draw_PrintNumberWidth3Unk(int arg0);
int Draw_GetBlendColor(void);

void Menu_DrawBlendColorChannelRow(int arg0) {
    int shift;
    int value;

    Draw_StatePush();
    shift = arg0 << 3;
    Draw_SetColor(0x80 << shift);
    Draw_PrintTextById(arg0 + 0x35);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(0x0A, 3);
    value = (Draw_GetBlendColor() >> shift) & 0xFF;
    Draw_PrintNumberWidth3Unk((value - 0x20) >> 1);
    Draw_StatePop();
}
