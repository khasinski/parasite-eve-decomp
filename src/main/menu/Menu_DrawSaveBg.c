/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
extern int g_RenderBackBufferBase[];

void LoadImage(short *rect, int image);

void Menu_DrawSaveBg(void) {
    short rect[4];

    if (g_MenuSaveBgFadeState == 6) {
        rect[1] = 0x1E0;
        rect[3] = g_MenuSaveBgFadeHeight;
        rect[0] = 0;
        rect[2] = 0x100;
        LoadImage(rect, g_RenderBackBufferBase[0]);
    }
}
