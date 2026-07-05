/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

extern int g_DrawTextPosX;

int Str_LookupTable4(void);
void Draw_PrintCenteredTextInWidth(int arg0, int arg1);

int g_DrawPrimColor;
int g_DrawColorShaded;
int g_TextCursorX;
int g_TextCursorY;
int g_DrawTextPosX;
int D_8009D168;

void Draw_AllocColorTri(int arg0, int arg1, int arg2);

void MenuWidget_DestroyNodeRecursive(void *node);

void MenuWidget_DrawCenteredTableText(void) {
    Draw_PrintCenteredTextInWidth(Str_LookupTable4(), g_DrawTextPosX);
}

void Draw_DropShadow(void) {
    g_DrawPrimColor = 0xFF4000;
    g_DrawColorShaded = 0x7F2000;
    g_TextCursorX -= 2;
    g_TextCursorY -= 2;
    Draw_AllocColorTri(g_DrawTextPosX, D_8009D168, 0);
    g_DrawPrimColor = 0x808080;
    g_DrawColorShaded = 0x404040;
    g_TextCursorX += 2;
    g_TextCursorY += 2;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu32", Draw_SwapPrimBuffers);

void MenuWidget_DestroyPopupNode(void *node) {
    void *owner;

    owner = *(void **)((char *)node + 0x34);
    *(int *)((char *)owner + 0x80) = 0;
    MenuWidget_DestroyNodeRecursive(node);
}
