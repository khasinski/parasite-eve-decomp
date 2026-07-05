/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_widget.h"

void Draw_PushPrimToList(void *arg0);
void Draw_AllocColorGradient(int x, int y, int width, int cursor_y);

extern int g_DrawTextDimmed;
extern int g_TextCursorX;
extern int g_TextCursorY;
extern int g_DrawTextBoxWidth;

int g_TextCursorX;
int g_TextCursorY;

void MenuWidget_UpdateAndDraw(void) {
    MenuWidgetNode *node;
    int any_flag;
    int order_flag;

    node = g_MenuWidgetActiveListHead;
    order_flag = 0;
    any_flag = 0;
    if (node != 0) {
        do {
            if (node->mode == 1) {
                node->field_3C = 0;
                any_flag |= node->cursor_x;
            }
            node = node->next;
        } while (node != 0);
    }

    node = g_MenuWidgetCurrentNode;
    if (node != 0) {
        do {
            if (node->mode == 1) {
                node->field_3C = order_flag;
                order_flag = 1;
            }
            node = (MenuWidgetNode *)node->field_04;
        } while (node != 0);
    }

    node = g_MenuWidgetActiveListHead;
    if (node != 0) {
        do {
            int enabled;
            int extra;

            if (node->mode == 1 && node->cursor_y == 0) {
                if (node->field_40 != 0) {
                    enabled = 0;
                } else {
                    enabled = node->field_3C & 1;
                }
                node->field_3C = enabled;

                extra = 0;
                if (any_flag != 0) {
                    extra = (unsigned int)node->cursor_x < 1;
                }

                node->field_3C = enabled | extra;
                g_DrawTextDimmed = enabled | extra;
                g_TextCursorX = 0;
                g_TextCursorY = 0;
                g_DrawTextBoxWidth = node->grid_width;
                Draw_PushPrimToList(node);
                g_TextCursorX = node->value_18;
                g_TextCursorY = node->value_1C;
                g_DrawTextDimmed = node->field_3C;
                Draw_AllocColorGradient(node->grid_width, node->visible_rows, node->target_x, node->cursor_x);
            }
            node = node->next;
        } while (node != 0);
    }
}

void MenuWidget_OffsetPosition(MenuWidgetNode *ptr, int dx, int dy) {
    if (ptr != 0) {
        ptr->value_18 += dx;
        ptr->value_1C += dy;
        g_TextCursorX += dx;
        g_TextCursorY += dy;
    }
}
