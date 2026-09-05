#include "pe1/menu_widget.h"

void MenuWidget_NavScrollTo(int selected_base);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);

extern char g_MenuTwoLineDialogText[];
extern char D_800A1A20[];

void Draw_OffsetCursor(int x, int y);
void Draw_PrintCenteredText(char *text);
int Draw_MeasureTextWidth(char *text);
void Menu_DrawTwoLineDialogText(void);
void Menu_SetDeferredCallback(void (*callback)(void));
int Menu_HandleDeferredCallbackInput(int arg0, int arg1);
void Menu_DrawNotificationDialogContent(void);
char *Str_LookupTable4(int id);
void Util_CopyFFTerminatedBytes(char *dst, char *src);
void Queue_Init(void);

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}

void Menu_DrawNotificationText(MenuWidgetNode *node) {
    char *text;

    Draw_OffsetCursor(0, 0xA);
    text = g_MenuTwoLineDialogText;
    if (node->selected_base == 0x3D) {
        text += 0x40;
    }
    Draw_PrintCenteredText(text);
}

void Menu_CreateTwoLineDialog(int line0_id, int line1_id) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    char *line0;
    char *line1;
    char *text;
    int width;
    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->field_30 = Menu_DrawTwoLineDialogText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->field_30 = Menu_DrawNotificationDialogContent;

    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);

    text = Str_LookupTable4(line0_id);
    line0 = D_800A1A20;
    Util_CopyFFTerminatedBytes(line0, text);

    text = Str_LookupTable4(line1_id);
    line1 = line0 + 0x40;
    Util_CopyFFTerminatedBytes(line1, text);

    if (Draw_MeasureTextWidth(line0) > Draw_MeasureTextWidth(line1)) {
        if (Draw_MeasureTextWidth(line0) >= 100)
            goto measure_wide;
        width = 100;
        /* Keep the short-width branch distinct from the second-line test. */
        asm("");
        goto width_ready;
    }
    if (Draw_MeasureTextWidth(line1) < 100) {
        width = 100;
        goto width_ready;
    }
measure_wide:
    {
        char *first = D_800A1A20;
        char *second = first + 0x40;
        width = Draw_MeasureTextWidth(first) > Draw_MeasureTextWidth(second)
            ? Draw_MeasureTextWidth(first) : Draw_MeasureTextWidth(second);
    }
width_ready:
    /* Do not pull layout arithmetic into the preceding jump delay slot. */
    asm("");
    parent->grid_width = width + 0x14;
    parent->x = (0x12C - width) >> 1;
    parent->visible_rows += 0xE;
    child->x = parent->grid_width - 0x44;
    child->y += 0xE;

    Queue_Init();
}

void Menu_DrawTwoLineDialogText(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText);
    Draw_OffsetCursor(0, 0xE);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText + 0x40);
}
