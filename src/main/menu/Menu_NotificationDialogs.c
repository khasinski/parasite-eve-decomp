#include "pe1/menu_dialog.h"
#include "pe1/menu_queue.h"
#include "pe1/text.h"

void Menu_CreateNotificationDialog(int message, int suffix) {
    MenuWidgetNode *parent, *child;
    short alternate;
    int mode, width;
    u8 *text, *messageText;
    alternate = MenuWidget_FindByModeAndSelectedBase(1, 0x28) != 0;
    mode = 0x28;
    if (alternate) {
        mode = 0x3D;
    }
    parent = MenuWidget_CreateSimpleNode(mode, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(mode, parent, parent);
    parent->field_30 = Menu_DrawNotificationText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->field_30 = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);
    messageText = Str_LookupTable4(message);
    text = D_800A1A20[alternate];
    Util_CopyFFTerminatedBytes(text, messageText);
    if (suffix) {
        Util_AppendFFTerminatedBytes(text, Str_LookupTable4(suffix));
    }
    width = Draw_MeasureTextWidth(text) < 100 ? 100 : Draw_MeasureTextWidth(text);
    parent->grid_width = width + 20;
    parent->x = (300 - width) >> 1;
    child->x = parent->grid_width - 68;
    Queue_Init();
}

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}

void Menu_DrawNotificationText(MenuWidgetNode *node) {
    u8 *text;

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
    u8 *line0;
    u8 *line1;
    u8 *text;
    int width;
    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->field_30 = Menu_DrawTwoLineDialogText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->field_30 = Menu_DrawNotificationDialogContent;

    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);

    text = Str_LookupTable4(line0_id);
    line0 = D_800A1A20[0];
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
        u8 *first = D_800A1A20[0];
        u8 *second = first + 0x40;
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
