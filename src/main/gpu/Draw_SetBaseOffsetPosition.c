extern signed char D_800B0DB0[];
extern signed char g_DrawBaseY[];

void Draw_SetBaseY(int arg0, int arg1);

void Draw_SetBaseOffsetPosition(int x, int y) {
    Draw_SetBaseY(D_800B0DB0[0] + x, g_DrawBaseY[0] + y);
}
