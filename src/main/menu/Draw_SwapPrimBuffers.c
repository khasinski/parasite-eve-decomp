/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

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
    node->field_28 = 0;
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
