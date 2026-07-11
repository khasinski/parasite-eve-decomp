#include "pe1/menu_widget.h"

int MenuWidget_GetCellIndex(MenuWidgetNode *ptr) {
    int idx;
    int ret;
    int x;
    int y;

    idx = -1;
    if (ptr != 0) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            idx = y;
            if (y >= 0) {
                idx = (ptr->grid_width * y) + x;
            } else {
                idx = -1;
            }
        }
    }

    ret = -1;
    if (((ptr->cell_mask >> idx) & 1) != 0) {
        ret = idx;
    }
    return ret;
}
