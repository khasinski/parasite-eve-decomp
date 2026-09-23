/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_widget.h"

extern int D_8009CF0C, D_8009CF30, D_8009CF34, D_8009CF1C;
MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *, int);
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int, int);
void MenuWidget_OffsetPosition(MenuWidgetNode *, int, int);
void MenuWidget_ClearCursorY(MenuWidgetNode *);
void MenuWidget_ClampScroll(MenuWidgetNode *);
void MenuWidget_SetCurrentNode(MenuWidgetNode *);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int, MenuWidgetNode *, int, int);
MenuWidgetNode *MenuWidget_CreateNode(int, MenuWidgetNode *, MenuWidgetNode *);
void MenuWidget_DestroyNode(MenuWidgetNode *);
void MenuWidget_NavScrollTo(int);
int Inv_GetPackedListCount(void);
void Menu_SetupSkillSubmenu(int);
void Menu_PlayConfirmSound(void);
void Menu_PlayErrorSound(void);
void Menu_PlayCancelSound(void);
void Menu_StepInventoryRoot(int, int, int);
void Menu_OpenEquipScreen(int);

int Menu_InventoryPageNavHandler(MenuWidgetNode *root, unsigned int flags) {
    MenuWidgetNode *node;
    register MenuWidgetNode *other asm("$16");
    register int offset asm("$18");
    register int present asm("$21");
    MenuWidgetNode *current;
    register MenuWidgetNode *child asm("$3");
    register int x asm("$2");
    register int oldx asm("$5");
    MenuWidget_GetChild(root, 0);
    if (flags & 0x14000) {
        if (Inv_GetPackedListCount() != 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 7);
            present = node != 0;
            if (node == 0) {
                node = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
                other = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
                child = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
                oldx = other->x;
                x = D_8009CF30;
                if (x == 0) {
                    x = (int)child->popup_node;
                    if (x == 0) x = 0x9C;
                    else x = 0xA2;
                } else x = 0xB0;
                offset = x - oldx;
                /* Keep the subtraction ahead of the first call argument. */
                asm volatile("" : : "r"(offset));
                MenuWidget_OffsetPosition(other, offset, 0);
                MenuWidget_ClearCursorY(other);
                other = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
                MenuWidget_OffsetPosition(other, offset, 0);
                MenuWidget_ClearCursorY(other);
                other = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
                MenuWidget_OffsetPosition(other, offset, 0);
                MenuWidget_ClearCursorY(other);
            }
            node->cursor_x = 0;
            node->cursor_y = 0;
            MenuWidget_ClampScroll(node);
            MenuWidget_SetCurrentNode(node);
            if (present) {
                root = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                MenuWidget_CreateNode(0x35, root, root)->draw = (void (*)())Menu_SetupSkillSubmenu;
            }
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }
    if (flags & 0x40) {
        MenuWidget_DestroyNode(root);
        if (MenuWidget_FindByModeAndSelectedBase(1, 5)) {
            MenuWidget_NavScrollTo(5);
            MenuWidget_NavScrollTo(6);
            MenuWidget_NavScrollTo(7);
            MenuWidget_NavScrollTo(0x35);
            Menu_StepInventoryRoot(0x33E, -1, -1);
        } else {
            MenuWidget_NavScrollTo(0xF);
            MenuWidget_NavScrollTo(0xB);
            MenuWidget_NavScrollTo(0xD);
            MenuWidget_NavScrollTo(0x18);
            MenuWidget_NavScrollTo(0x30);
            if (D_8009CF0C) {
                MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
            } else {
                current = MenuWidget_GetCurrentNode();
                if (current) MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
                MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
            }
            D_8009CF34 = 0;
            MenuWidget_NavScrollTo(0x2F);
            D_8009CF1C = 0;
            D_8009CF30 = 0;
            Menu_OpenEquipScreen(D_8009CF0C - 1);
        }
        Menu_PlayCancelSound();
    }
    return 1;
}
