#include "pe1/menu_widget.h"
/* MASPSX_FLAGS: --expand-div */

#define NULL ((void *)0)
void MenuWidget_EaseNodePosition(MenuWidgetNode *arg0) {
    s32 temp_a2;
    s32 temp_v1;
    s32 temp_v1_2;
    MenuWidgetNode *temp_a1;

    temp_a1 = ((MenuWidgetListNavigation *)arg0)->list;
    if (arg0 != NULL) {
        temp_v1 = temp_a1->visible_rows;
        temp_a2 = temp_a1->y_limit;
        if (temp_v1 < temp_a2) {
            arg0->draw_state = ((temp_a1->disabled * temp_v1 * temp_v1) / temp_a2);
            temp_v1_2 = temp_a1->disabled;
            arg0->visible_rows = ((temp_v1_2 * temp_a1->visible_rows * ((temp_v1_2 * temp_a1->scroll_y) - temp_a1->scroll_adjust)) / (temp_a1->y_limit * temp_v1_2));
        }
    }
}
