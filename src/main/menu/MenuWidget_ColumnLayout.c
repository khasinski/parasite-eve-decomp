/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/menu_widget.h"

int g_MenuWidgetColumnLayoutMode;
extern MenuWidgetSavedLayout g_MenuWidgetColumnLayoutTable[];
void bzero(void *ptr, int size);
extern int g_DrawTextPosX;

/* Saved cursor/scroll positions of menu widgets (one 4-byte slot per
 * aux_index): reset, mode, save and restore. Sys_InitStateBuffer (historical
 * name) is the unconditional reset; the eight slots set to -1 start with no
 * saved column. */

void Sys_InitStateBuffer(void) {
    bzero(g_MenuWidgetColumnLayoutTable, 0x120);
    g_MenuWidgetColumnLayoutTable[6].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[16].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[20].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[22].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[24].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[25].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[49].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[53].cursorX = -1;
}

void MenuWidget_SetColumnLayoutMode(int arg0) {
    g_MenuWidgetColumnLayoutMode = arg0;
    if (arg0 == 0) {
        bzero(g_MenuWidgetColumnLayoutTable, 0x120);
        g_MenuWidgetColumnLayoutTable[6].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[16].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[20].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[22].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[24].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[25].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[49].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[53].cursorX = -1;
    }
}

int MenuWidget_GetColumnLayoutMode(void) {
    return g_MenuWidgetColumnLayoutMode;
}

void MenuWidget_SaveColumnLayout(MenuWidgetNode *node) {
    int index;
    MenuWidgetSavedLayout *slot;

    if (g_MenuWidgetColumnLayoutMode == 0 && ((node->layout_flags & 0x20) == 0)) {
        return;
    }

    index = node->aux_index;
    if ((unsigned int)index < 0x48) {
        slot = &g_MenuWidgetColumnLayoutTable[index];
        slot->cursorX = node->cursor_x;
        slot->cursorY = node->cursor_y;
        slot->scrollY = node->scroll_y;
    }
}

void MenuWidget_ApplyColumnLayout(MenuWidgetNode *node) {
    MenuWidgetSavedLayout *entry;
    int flag;

    if (node == 0) {
        return;
    }
    if (node->aux_index < 0) {
        return;
    }

    entry = &g_MenuWidgetColumnLayoutTable[node->aux_index];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry->cursorX;
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry->cursorY;
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry->scrollY;
    }
}

void MenuWidget_SetColumnLayout(MenuWidgetNode *node, int arg1) {
    MenuWidgetSavedLayout *entry;
    int flag;

    node->aux_index = arg1;
    if (node == 0) {
        return;
    }
    if (arg1 < 0) {
        return;
    }

    entry = &g_MenuWidgetColumnLayoutTable[arg1];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry->cursorX;
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry->cursorY;
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry->scrollY;
    }
}

void MenuWidget_ClearColumnLayout(MenuWidgetNode *node) {

    node->aux_index = -1;
    node->cursor_x = -1;
}

void MenuWidget_DrawCenteredText(u8 *text) {
    Draw_PrintCenteredTextInWidth(text, g_DrawTextPosX);
}

#include "pe1/draw_state.h"
#include "pe1/text.h"

extern int D_8009D164;

void MenuWidget_DrawCenteredTableText(int text_id) {
    Draw_PrintCenteredTextInWidth(Str_LookupTable4(text_id), D_8009D164);
}
