/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef short s16;

extern int D_8009D0FC;
extern int g_DrawBufferIndex;
extern int D_8009D118;
extern int g_DrawPresentEnabled;
extern int g_DrawPresentImage;

int VSync(int arg0);
void DrawSync(int arg0);
void Render_InitEntityPool(int arg0);
void PutDrawEnv(int arg0);
void PutDispEnv(int arg0) __asm__("Render_StepEntityPool");
void LoadImage(s16 *rect, int image);
void DrawOTag(int arg0);

void Draw_PresentFrame(int arg0) {
    register int index asm("$16");
    s16 rect[4];
    int mode;
    int image;
    int y;

    index = arg0;
    if (g_DrawPresentEnabled == 0) {
        return;
    }

    VSync(1);
    DrawSync(0);
    mode = index;
    index = 0xFFF;
    if (mode == 1) {
        mode = 0;
    }
    VSync(mode);
    Render_InitEntityPool(1);
    PutDrawEnv(D_8009D0FC);
    PutDispEnv(D_8009D0FC + 0x5C);

    image = g_DrawPresentImage;
    if (image != 0) {
        y = 0xB;
        rect[0] = 0;
        if (g_DrawBufferIndex != 0) {
            y = 0xEB;
        }
        rect[1] = y;
        rect[2] = 0x140;
        rect[3] = 0xCC;
        LoadImage(rect, image);
    }

    {
        register int offset asm("$4");

        offset = index * 4;
        DrawOTag(D_8009D118 + offset);
    }
}
