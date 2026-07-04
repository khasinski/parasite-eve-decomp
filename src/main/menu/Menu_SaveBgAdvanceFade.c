/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
typedef short s16;
typedef unsigned int u32;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
extern u32 g_RenderFrontBufferBase[];

int StoreImage(RECT *rect, int image);
void Menu_SaveBgApplyFadeStep(void);

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
