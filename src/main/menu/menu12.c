/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_widget.h"

typedef struct MenuData {
    char pad0[0x14];
    unsigned char count;
    unsigned char entries[1];
} MenuData;

extern MenuWidgetNode *MenuWidget_GetCurrentNode(void);
extern MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *arg1, int arg2, int arg3);
extern MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
extern void Menu_DrawItemListHeader(void);
extern int Menu_HandleDeferredCallbackInput(int arg0, int arg1);
extern void Menu_DrawNotificationDialogContent(void);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void Menu_SetDeferredCallback(void (*callback)(void));
extern void Inv_SelectActiveList(int arg0);
extern MenuData *Inv_LookupActiveListData(int arg0);

extern signed char g_AyaEquippedArmorSlot[];
extern int D_8009CFC0;

int D_8009CFC0;

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_AllocSprite(int arg0);
void Draw_PrintTextById(int arg0);

void Menu_CreateItemList(void) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    MenuData *data;
    int i;
    int value;

    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->field_30 = Menu_DrawItemListHeader;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->field_30 = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);

    Menu_SetDeferredCallback(0);

    parent->grid_width = 0xDC;
    parent->value_18 = 0x32;
    parent->visible_rows += 0xA;

    child->value_18 = parent->grid_width - 0x44;
    child->value_1C += 8;

    D_8009CFC0 = 0x37;

    if (g_AyaEquippedArmorSlot[0] >= 0) {
        Inv_SelectActiveList(0);
        data = Inv_LookupActiveListData(g_AyaEquippedArmorSlot[0]);

        for (i = 0; i < data->count; i++) {
            if ((data->entries[i] & 0xE0) == 0xA0) {
                break;
            }
        }

        value = D_8009CFC0 - 1;
        D_8009CFC0 = value + (data->entries[i] & 0x1F);
    }
}

void Menu_DrawItemListHeader(void) {
    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(D_8009CFC0);
    Draw_OffsetCursor(0x20, 0);
    Draw_PrintTextById(0x1C);
    Draw_OffsetCursor(-0x20, 0x10);
    Draw_PrintTextById(0x1D);
}
