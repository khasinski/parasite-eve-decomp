/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/psyq_gpu.h"

typedef unsigned int u32;

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
extern u32 g_RenderFrontBufferBase[];

int StoreImage(RECT *rect, u32 *p);
void Menu_SaveBgApplyFadeStep(void);

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
extern int g_RenderBackBufferBase[];

int LoadImage(RECT *rect, u32 *p);

void Menu_SaveBgAdvanceFade(void) {
    RECT rect;
    int next;

    switch (g_MenuSaveBgFadeState) {
    case 0:
        return;

    case 1:
        next = 2;
        break;

    case 2:
        next = 3;
        break;

    case 3:
        rect.y = 0x1E0;
        rect.x = 0;
        rect.w = 0x100;
        rect.h = g_MenuSaveBgFadeHeight;
        StoreImage(&rect, g_RenderFrontBufferBase[0]);
        next = 4;
        break;

    case 5:
        g_MenuSaveBgFadeState = 6;
        Menu_SaveBgApplyFadeStep();
        return;

    case 4:
    case 6:
        next = 5;
        break;

    case 7:
        return;

    default:
        return;
    }

    g_MenuSaveBgFadeState = next;
}

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
