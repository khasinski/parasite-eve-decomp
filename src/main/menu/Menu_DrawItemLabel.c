extern char g_MenuItemLabelBuffer[];
extern int g_MenuItemPrimListHandles[];

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_SetTextDimmed(int arg0);
void Draw_PrintCenteredText(void *arg0);
void Draw_FlushPrimList(int arg0);

#include "pe1/menu_widget.h"

void Menu_DrawItemLabel(MenuWidgetNode *arg0) {
    int idx;

    idx = arg0->selected_base - 0x29;
    Draw_OffsetCursor(0, 0xA);
    Draw_SetTextDimmed(0);
    Draw_PrintCenteredText(g_MenuItemLabelBuffer + (idx << 6));
    if (g_MenuItemPrimListHandles[idx] != 0) {
        Draw_OffsetCursor(0, 0xE);
        Draw_FlushPrimList(g_MenuItemPrimListHandles[idx]);
    }
}
