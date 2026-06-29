void Draw_OffsetCursor(int arg0, int arg1);
int Draw_GetBaseY(void);
void Draw_PrintSignedNumberWidth3(int arg0);
int VSync(int arg0);
void Draw_SetTextDimmed(int arg0);
void Draw_AllocSprite(int arg0);
void Draw_EmitGlyph(int arg0, int arg1);

void Menu_DrawScreenAdjustPanel(void) {
    Draw_OffsetCursor(0x10, 0xA);
    Draw_PrintSignedNumberWidth3(8 - Draw_GetBaseY());
    Draw_SetTextDimmed(VSync(-1) & 0x10);
    Draw_OffsetCursor(-0x10, -0x69);
    Draw_AllocSprite(0x7B);
    Draw_OffsetCursor(0, 0xBE);
    Draw_EmitGlyph(0x7B, 2);
}
