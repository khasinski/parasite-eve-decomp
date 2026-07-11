#include "pe1/menu_widget.h"

int MenuWidget_GridCellIndex(MenuWidgetNode *ptr) {
    int ret;
    int x;
    int y;

    ret = -1;
    if (ptr != 0) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            if (y >= 0) {
                ret = (ptr->grid_width * y) + x;
            }
        }
    }

    return ret;
}
