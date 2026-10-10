/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */

#include "pe1/menu_inventory.h"

MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetFreeListHead;

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

static inline MenuWidgetNode *Allocate(MenuWidgetNode *parent,
                                       MenuWidgetNode *owner)
{
    MenuWidgetNode *node = g_MenuWidgetFreeListHead;
    MenuWidgetNode *next;
    MenuWidgetNode *old;
    int i;

    if (!node)
        BoundsCheck_AssertStub(10);
    next = node->next;
    old = g_MenuWidgetActiveListHead;
    g_MenuWidgetActiveListHead = node;
    node->parent = parent;
    node->update = 0;
    node->draw = 0;
    g_MenuWidgetFreeListHead = next;
    node->next = old;
    for (i = 3; i >= 0; i--)
        node->children[i] = 0;
    node->y = 0;
    node->x = 0;
    node->selected_base = 0;
    node->mode = 0;
    node->flags = 0;
    if (owner) {
        for (i = 0; i < 4; i++) {
            if (!owner->children[i])
                break;
        }
        if (i < 4)
            owner->children[i] = node;
        else
            BoundsCheck_AssertStub(11);
    }
    return node;
}

/* Historical symbol name: constructs a mode-3 list-navigation widget. */
void Draw_SwapPrimBuffers(MenuWidgetNode *list)
{
    MenuWidgetNode *owner = FindOwner(list);
    MenuWidgetNode *node = Allocate(list->parent, owner);
    MenuWidgetListNavigation *navigation = (MenuWidgetListNavigation *)node;

    if (!node)
        BoundsCheck_AssertStub(16);
    node->mode = 3;
    node->draw = Draw_FlushFrontBuffer;
    node->update = (void (*)())Menu_StepListNavigate;
    navigation->list = list;
    navigation->flags = list->layout_flags;
    list->popup_node = node;
}


#include "pe1/menu_widget.h"

void MenuWidget_DestroyPopupNode(MenuWidgetNode *node) {
    MenuWidgetNode *owner = ((MenuWidgetListNavigation *)node)->list;

    owner->popup_node = 0;
    MenuWidget_DestroyNodeRecursive(node);
}

#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"
#include "pe1/textbox.h"
#include "pe1/psyq_cd.h"

MenuWidgetNode *g_MenuWidgetActiveListHead, *g_MenuWidgetCurrentNode;
int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;


static inline void Offset(int x, int y) {
    g_DrawSpriteX += x;
    g_DrawSpriteY += y;
}
static inline void PopCursor(void) {
    if (g_TextCursorStackBottom < g_TextCursorStack) {
        int x = ((DrawTextCursorPair *)g_TextCursorStack)[-1].x;
        int y = ((DrawTextCursorPair *)g_TextCursorStack)[-1].y;
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
        DrawTextCursorPair *stack;
        g_DrawColorSelect = owner->mode == 1 && owner->draw_state != 0;
        stack = (DrawTextCursorPair *)g_TextCursorStack;
        if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
            int x = g_DrawSpriteX;
            int y = g_DrawSpriteY;
            g_TextCursorStack = (int *)(stack + 1);
            stack->x = x;
            stack->y = y;
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

#include "pe1/menu_inventory.h"

static inline int Move(MenuWidgetNode *node, int delta) {
    int changed = 0;

    if (!node->scroll_adjust) {
        int old = node->scroll_y;
        node->scroll_y += delta;
        if (node->scroll_y < 0) {
            node->scroll_y = 0;
        } else if (node->scroll_y > node->y_limit - node->visible_rows) {
            node->scroll_y = node->y_limit - node->visible_rows;
        }
        changed = old != node->scroll_y;
        if (changed) {
            int speed = node->disabled;
            node->scroll_adjust = (delta > 0 ? speed : -speed) / 2;
        }
    }
    return changed;
}

int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags) {
    MenuWidgetNode *list = node->list;
    int handled = 0;

    if (flags & 0x1004) {
        if (Move(list, -list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    } else if (flags & 0x4008) {
        if (Move(list, list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    }
    return handled || !(node->flags & 0x40);
}

#include "pe1/menu_widget.h"

#define NULL ((void *)0)
void MenuWidget_EaseNodePosition(MenuWidgetNode *arg0) {
    s32 temp_a2;
    s32 temp_v1;
    s32 temp_v1_2;
    MenuWidgetNode *temp_a1;
    MenuWidgetListNavigation *navigation = (MenuWidgetListNavigation *)arg0;

    temp_a1 = navigation->list;
    if (arg0 != NULL) {
        temp_v1 = temp_a1->visible_rows;
        temp_a2 = temp_a1->y_limit;
        if (temp_v1 < temp_a2) {
            navigation->drawState = ((temp_a1->disabled * temp_v1 * temp_v1) / temp_a2);
            temp_v1_2 = temp_a1->disabled;
            navigation->visibleRows = ((temp_v1_2 * temp_a1->visible_rows * ((temp_v1_2 * temp_a1->scroll_y) - temp_a1->scroll_adjust)) / (temp_a1->y_limit * temp_v1_2));
        }
    }
}
