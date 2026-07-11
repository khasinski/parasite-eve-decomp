/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

typedef unsigned char u8;

extern MenuWidgetNode *MenuWidget_GetCurrentNode(void);
extern MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
extern MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void Menu_SetDeferredCallback(void (*callback)(void));
extern int Menu_HandleDeferredCallbackInput(int arg0, int arg1);
extern void Menu_DrawNotificationText(void);
extern void Menu_DrawNotificationDialogContent(void);
extern u8 *Str_LookupTable4(int id);
extern void Util_CopyFFTerminatedBytes(u8 *dst, u8 *src);
extern void Util_AppendFFTerminatedBytes(u8 *dst, u8 *src);
extern int Draw_MeasureTextWidth(u8 *text);
extern void Queue_Init(void);

extern u8 D_800A1A20[];

void Menu_CreateNotificationDialog(int line0_id, int line1_id) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    u8 *text;
    int has_existing;
    int mode;
    int width;

    has_existing = MenuWidget_FindByModeAndSelectedBase(1, 0x28) != 0;
    mode = 0x28;
    if (has_existing != 0) {
        mode = 0x3D;
    }

    parent = MenuWidget_CreateSimpleNode(mode, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(mode, parent, parent);

    parent->field_30 = Menu_DrawNotificationText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->field_30 = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);

    text = D_800A1A20 + has_existing * 0x40;
    Util_CopyFFTerminatedBytes(text, Str_LookupTable4(line0_id));
    if (line1_id != 0) {
        Util_AppendFFTerminatedBytes(text, Str_LookupTable4(line1_id));
    }

    width = 0x64;
    if (Draw_MeasureTextWidth(text) >= 0x64) {
        width = Draw_MeasureTextWidth(text);
    }

    parent->grid_width = width + 0x14;
    parent->value_18 = (0x12C - width) / 2;
    child->value_18 = parent->grid_width - 0x44;
    Queue_Init();
}
