/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/menu_widget.h"

int g_MenuWidgetColumnLayoutMode;
extern s8 g_MenuWidgetColumnLayoutTable[][4];
void bzero(void *ptr, int size);
extern int g_DrawTextPosX;
void Draw_PrintCenteredTextInWidth(int arg0, int arg1);

/* Saved cursor/scroll positions of menu widgets (one 4-byte slot per
 * aux_index): reset, mode, save and restore. Sys_InitStateBuffer (historical
 * name) is the unconditional reset; the eight slots set to -1 start with no
 * saved column. */

void Sys_InitStateBuffer(void) {
    bzero(g_MenuWidgetColumnLayoutTable, 0x120);
    g_MenuWidgetColumnLayoutTable[6][0] = -1;
    g_MenuWidgetColumnLayoutTable[16][0] = -1;
    g_MenuWidgetColumnLayoutTable[20][0] = -1;
    g_MenuWidgetColumnLayoutTable[22][0] = -1;
    g_MenuWidgetColumnLayoutTable[24][0] = -1;
    g_MenuWidgetColumnLayoutTable[25][0] = -1;
    g_MenuWidgetColumnLayoutTable[49][0] = -1;
    g_MenuWidgetColumnLayoutTable[53][0] = -1;
}

void MenuWidget_SetColumnLayoutMode(int arg0) {
    g_MenuWidgetColumnLayoutMode = arg0;
    if (arg0 == 0) {
        bzero(g_MenuWidgetColumnLayoutTable, 0x120);
        g_MenuWidgetColumnLayoutTable[6][0] = -1;
        g_MenuWidgetColumnLayoutTable[16][0] = -1;
        g_MenuWidgetColumnLayoutTable[20][0] = -1;
        g_MenuWidgetColumnLayoutTable[22][0] = -1;
        g_MenuWidgetColumnLayoutTable[24][0] = -1;
        g_MenuWidgetColumnLayoutTable[25][0] = -1;
        g_MenuWidgetColumnLayoutTable[49][0] = -1;
        g_MenuWidgetColumnLayoutTable[53][0] = -1;
    }
}

int MenuWidget_GetColumnLayoutMode(void) {
    return g_MenuWidgetColumnLayoutMode;
}

void MenuWidget_SaveColumnLayout(MenuWidgetNode *node) {
    int index;
    signed char *slot;

    if (g_MenuWidgetColumnLayoutMode == 0 && ((node->layout_flags & 0x20) == 0)) {
        return;
    }

    index = node->aux_index;
    if ((unsigned int)index < 0x48) {
        slot = g_MenuWidgetColumnLayoutTable[index];
        slot[0] = node->cursor_x;
        slot[1] = node->cursor_y;
        slot[2] = node->scroll_y;
    }
}

void MenuWidget_ApplyColumnLayout(void *arg0) {
    MenuWidgetNode *node = arg0;
    signed char *entry;
    int flag;

    if (node == 0) {
        return;
    }
    if (node->aux_index < 0) {
        return;
    }

    entry = g_MenuWidgetColumnLayoutTable[node->aux_index];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry[0];
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry[1];
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry[2];
    }
}

void MenuWidget_SetColumnLayout(void *arg0, int arg1) {
    MenuWidgetNode *node = arg0;
    signed char *entry;
    int flag;

    node->aux_index = arg1;
    if (node == 0) {
        return;
    }
    if (arg1 < 0) {
        return;
    }

    entry = g_MenuWidgetColumnLayoutTable[arg1];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry[0];
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry[1];
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry[2];
    }
}

void MenuWidget_ClearColumnLayout(void *ptr) {
    MenuWidgetNode *node = ptr;

    node->aux_index = -1;
    node->cursor_x = -1;
}

void MenuWidget_DrawCenteredText(int arg0) {
    Draw_PrintCenteredTextInWidth(arg0, g_DrawTextPosX);
}
