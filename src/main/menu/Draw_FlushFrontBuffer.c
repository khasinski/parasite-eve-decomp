/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"
#include "pe1/textbox.h"
#include "pe1/psyq_cd.h"

MenuWidgetNode *g_MenuWidgetActiveListHead, *g_MenuWidgetCurrentNode;
int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

static inline MenuWidgetNode *FindOwner(MenuWidgetNode *node)
{
    MenuWidgetNode *owner = g_MenuWidgetActiveListHead;
    MenuWidgetNode *result = 0;

    while (owner) {
        int i;
        for (i = 0; i < 4; i++) {
            if (owner->children[i] == node)
                break;
        }
        if (i < 4) {
            result = owner;
            break;
        }
        owner = owner->next;
    }
    return result;
}

static inline void Offset(int x, int y) {
    g_DrawSpriteX += x;
    g_DrawSpriteY += y;
}
static inline void PopCursor(void) {
    if (g_TextCursorStackBottom < g_TextCursorStack) {
        int x = g_TextCursorStack[-2];
        int y = g_TextCursorStack[-1];
        g_TextCursorStack -= 2;
        g_DrawSpriteX = x;
        g_DrawSpriteY = y;
    } else {
        BoundsCheck_AssertStub(3);
    }
}

void Draw_FlushFrontBuffer(MenuWidgetListNavigation *node) {
    MenuWidgetNode *list = node->list;
    if (list) {
        MenuWidgetNode *owner = FindOwner((MenuWidgetNode *)node);
        int *stack;
        g_DrawColorSelect = owner->mode == 1 && owner->draw_state != 0;
        stack = g_TextCursorStack;
        if (stack < g_TextCursorStackTop) {
            int x = g_DrawSpriteX;
            int y = g_DrawSpriteY;
            g_TextCursorStack = stack + 2;
            stack[0] = x;
            stack[1] = y;
        } else {
            BoundsCheck_AssertStub(2);
        }
        Offset(list->draw_state * list->grid_width + 2,
               node->visibleRows + 2);
        if (g_MenuWidgetCurrentNode == (MenuWidgetNode *)node && (VSync(-1) & 8))
            Draw_AllocColorTri(8, node->drawState, 0);
        Draw_AllocColorGradient(8, node->drawState, 0, 1);
        if (list->scroll_y) {
            Offset(0, -6);
            Draw_AllocSprite(0x4a);
            Offset(0, 6);
        }
        if (list->scroll_y < list->y_limit - list->visible_rows) {
            Offset(0, node->drawState + 2);
            Draw_AllocSprite(0x4b);
        }
        PopCursor();
    }
}
