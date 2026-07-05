#include "include_asm.h"

#include "pe1/menu_widget.h"

int MenuWidget_GridCellIndex(MenuWidgetNode *ptr) {
    int ret;
    int x;
    int y;

    ret = -1;
    if (ptr) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            if (y >= 0) {
                ret = ptr->grid_width * y + x;
            }
        }
    }

    return ret;
}

int MenuWidget_GetCellIndex(void *w) {
    int idx = -1;
    int ret;

    if (w != 0) {
        int col = *(int *)((char *)w + 0x44);
        if (col >= 0) {
            int row = *(int *)((char *)w + 0x48);
            idx = row;
            if (row >= 0) {
                idx = (*(int *)((char *)w + 0x34) * row) + col;
            } else {
                idx = -1;
            }
        }
    }
    ret = -1;
    if ((*(int *)((char *)w + 0x74) >> idx) & 1) {
        ret = idx;
    }
    return ret;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_widget6", MenuWidget_DrawListRow);

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_widget6", Draw_AllocPrimWithMask);

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_widget6", MenuWidget_DrawList);

void MenuWidget_ClampScroll(MenuWidgetNode *ptr) {
    int y;
    int top;
    int height;
    int next;

    y = ptr->cursor_y;
    top = ptr->scroll_y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}

void MenuWidget_ClampCursor(MenuWidgetNode *ptr, int x, int y) {
    int limit;
    int top;
    int height;
    int next;

    if (x < 0) {
        x = 0;
    } else {
        limit = ptr->x_limit;
        if (x >= limit) {
            x = limit - 1;
        }
    }

    if (y < 0) {
        y = 0;
    } else {
        limit = ptr->y_limit;
        if (y >= limit) {
            y = limit - 1;
        }
    }

    top = ptr->scroll_y;
    ptr->cursor_x = x;
    ptr->cursor_y = y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}
