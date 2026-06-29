/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

int g_TextCursorX;
int g_TextCursorY;

void MenuWidget_OffsetPosition(MenuWidgetNode *ptr, int dx, int dy) {
    if (ptr != 0) {
        ptr->value_18 += dx;
        ptr->value_1C += dy;
        g_TextCursorX += dx;
        g_TextCursorY += dy;
    }
}
