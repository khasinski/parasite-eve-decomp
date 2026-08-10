#include "common.h"
/* MASPSX_FLAGS: --expand-div */

#include "pe1/menu_widget.h"

void Draw_SwapPrimBuffers(void);

void Draw_SetPrimCallback(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *temp_a1_2;
    s32 temp_a2;
    s32 temp_v1;
    s32 t58b;
    s32 t48;
    s32 t5C;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a1;
    s32 var_a2;
    MenuWidgetNode *temp_a1;

    temp_v1 = arg0->grid_width;
    arg0->y_limit = (s32) ((s32) ((arg1 + temp_v1) - 1) / temp_v1);
    var_a2 = 0;
    if (arg0->grid_width == 2) {
        var_a2 = arg1 & 1;
    }
    __asm__ volatile("" : : : "memory");
    t48 = arg0->cursor_y;
    temp_v1_2 = arg0->y_limit;
    arg0->has_scroll = var_a2;
    if (t48 >= temp_v1_2) {
        arg0->cursor_y = temp_v1_2 - 1;
    }
    if ((arg0->has_scroll != 0) && (arg0->cursor_x == 1) &&
        (arg0->cursor_y == (arg0->y_limit - 1))) {
        arg0->cursor_x = 0;
    }
    var_a1 = arg0->visible_rows_mirror;
    temp_v1_3 = arg0->y_limit;
    temp_a2 = arg0->visible_rows;
    if (temp_v1_3 < var_a1) {
        var_a1 = temp_v1_3;
        temp_v1_3 = *(volatile s32 *)&arg0->y_limit;
    }
    temp_v1_4 = temp_v1_3 - var_a1;
    arg0->visible_rows = var_a1;
    if (temp_v1_4 < arg0->scroll_y) {
        arg0->scroll_y = temp_v1_4;
    }
    temp_a1 = arg0->parent;
    if ((temp_a1 != 0) && (temp_a1->mode == 1)) {
        temp_a1->visible_rows += arg0->disabled * (arg0->visible_rows - temp_a2);
    }
    temp_a1_2 = arg0->popup_node;
    if (temp_a1_2 != 0) {
        t58b = arg0->y_limit;
        if (arg0->visible_rows_mirror >= t58b) {
            MenuWidget_DestroyPopupNode(temp_a1_2);
        }
    } else {
        t58b = arg0->y_limit;
        if (arg0->visible_rows_mirror < t58b) {
            Draw_SwapPrimBuffers();
        }
    }
}
