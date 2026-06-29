#include "pe1/menu_widget.h"

void MenuWidget_SetCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 1;
    }
}
