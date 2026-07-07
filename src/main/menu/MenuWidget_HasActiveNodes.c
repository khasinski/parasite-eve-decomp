/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *g_MenuWidgetActiveListHead;

int MenuWidget_HasActiveNodes(void) {
    return g_MenuWidgetActiveListHead != 0;
}
