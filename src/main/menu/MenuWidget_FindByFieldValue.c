/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_FindByFieldValue(int value) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;
    int i;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        i = 0;
        while (i < 4) {
            if (((int *)&node->field_08)[i] == value) {
                break;
            }
            i++;
        }
        if (i < 4) {
            result = node;
            break;
        }
        node = node->next;
    }
    return result;
}
