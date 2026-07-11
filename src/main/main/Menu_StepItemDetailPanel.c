/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

extern int D_8009CF04;
extern int D_8009CF10;
extern int D_8009CF14;
extern int D_8009CFA0;
extern void (*D_8009CFA8)(void);
extern unsigned char D_800A1980[];

void Inv_SelectActiveList(int list);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
void *Inv_LookupActiveListDisplayData(int index);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Menu_DrawItemLabel(void);
int Menu_ConfirmDialogHandler(void);
void Menu_DrawActionOptionList(void);
void Menu_OnItemDiscardConfirm(void);
void Util_CopyFFTerminatedBytes(unsigned char *dst, void *src);
void Util_AppendFFTerminatedBytes(unsigned char *dst, void *src);
void *Str_LookupTable4(int index);
int Draw_MeasureTextWidth(unsigned char *text);

void Menu_StepItemDetailPanel(void) {
    MenuWidgetNode *current;
    MenuWidgetNode *label_node;
    MenuWidgetNode *list_node;
    void *text;
    int width;

    Inv_SelectActiveList(D_8009CF10);
    current = MenuWidget_GetCurrentNode();
    text = Inv_LookupActiveListDisplayData(D_8009CF04);

    label_node = MenuWidget_CreateSimpleNode(0x29, current, 0, 1);
    list_node = MenuWidget_CreateNode(0x29, label_node, label_node);

    label_node->field_30 = Menu_DrawItemLabel;
    label_node->update = Menu_ConfirmDialogHandler;
    list_node->field_30 = Menu_DrawActionOptionList;
    D_8009CF14 = 5;

    MenuWidget_SetCurrentNode(list_node);
    list_node->cursor_x = 1;

    Inv_SelectActiveList(D_8009CF10);
    if (text != 0) {
        Util_CopyFFTerminatedBytes(D_800A1980, text);
    } else {
        D_800A1980[0] = 0xFF;
    }

    Util_AppendFFTerminatedBytes(D_800A1980, Str_LookupTable4(4));
    D_8009CFA0 = 0;

    width = 0x78;
    if (Draw_MeasureTextWidth(D_800A1980) >= 0x78) {
        width = Draw_MeasureTextWidth(D_800A1980);
    }

    label_node->grid_width = width + 0x14;
    label_node->visible_rows = 0x32;
    label_node->value_18 = (0x12C - width) >> 1;

    list_node->value_18 = (label_node->grid_width - 0x80) >> 1;
    list_node->value_1C = label_node->visible_rows - 0x14;
    D_8009CFA8 = Menu_OnItemDiscardConfirm;
}
