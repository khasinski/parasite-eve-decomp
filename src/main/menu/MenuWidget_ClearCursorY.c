#include "pe1/menu_widget.h"

void MenuWidget_ClearCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 0;
    }
}
