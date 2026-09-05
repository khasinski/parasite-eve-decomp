/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_inventory.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int kind, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int kind, MenuWidgetNode *parent, MenuWidgetNode *sibling);
void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *node);
int MemCard_GetActivePort(void);
int Str_LookupTable4(int index);
void Util_CopyFFTerminatedBytes(int dst, int src);
void Util_AppendFFTerminatedBytes(int dst, int src);
int Draw_MeasureTextWidth(int str);
void Inv_SelectActiveList(int list);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void MemCard_ClearActivePrompt(void);
void Menu_DrawItemLabel(void);
void Menu_ConfirmDialogHandler(void);
void Menu_DrawActionOptionList(void);
void Menu_HandleMemCardWriteOrError(int arg0, int arg1);

extern int D_8009CF50;
extern int D_8009CF14;
extern int D_8009CF10;
extern int D_8009CFA4;
extern int D_8009CFA8;
extern int D_8009CFFC;
extern unsigned char D_800A19C0[];
/* Separate compiler identities preserve address reloads across text calls. */
extern unsigned char D_800A19C0_measure[] asm("D_800A19C0");
extern unsigned char D_800A19C0_remeasure[] asm("D_800A19C0");

int Menu_MemCardProgressInputHandler(void) {
    return 1;
}

void Menu_StepItemGrid2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *option_node;
    int label;
    int suffix_id;
    int callback;
    register int mode asm("$4");
    int width;
    int height;

    if (D_8009CF50 != 0) {
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x2A) != 0) {
            return;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, 0x24);
        label = Str_LookupTable4(MemCard_GetActivePort() + 0x47);
        {
            MenuWidgetNode *result = MenuWidget_CreateSimpleNode(0x2A, node, 0, 1);
            mode = 0x2A;
            /* Keep the kind argument ready before copying the returned node. */
            asm("" : : "r"(result), "r"(mode));
            node = result;
        }
        option_node = MenuWidget_CreateNode(mode, node, node);
        node->field_30 = Menu_DrawItemLabel;
        node->update = Menu_ConfirmDialogHandler;
        option_node->field_30 = Menu_DrawActionOptionList;
        D_8009CF14 = 0x6C;
        MenuWidget_SaveAndSetCurrentNode(option_node);

        {
            int enabled = 1;
            int active_list;
            suffix_id = 0x4A;
            active_list = D_8009CF10;
            callback = (int)Menu_HandleMemCardWriteOrError;
            option_node->cursor_x = enabled;
            Inv_SelectActiveList(active_list);
        }

        if (label != 0) {
            Util_CopyFFTerminatedBytes((int)D_800A19C0, label);
        } else {
            D_800A19C0[0] = 0xFF;
        }
        {
            int text_buf;
            Util_AppendFFTerminatedBytes((int)D_800A19C0, Str_LookupTable4(0x49));
            D_8009CFA4 = suffix_id;
            text_buf = (int)D_800A19C0_measure;
            width = Draw_MeasureTextWidth(text_buf);
            if (width < 0x78) {
                width = 0x78;
            } else {
                text_buf = (int)D_800A19C0_remeasure;
                width = Draw_MeasureTextWidth(text_buf);
            }
        }

        node->grid_width = width + 0x14;
        node->visible_rows = 0x42;
        node->x = (0x12C - width) >> 1;
        option_node->x = (node->grid_width - 0x80) >> 1;
        height = node->visible_rows;
        D_8009CFA8 = callback;
        option_node->y = height - 0x14;
        return;
    }

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
        Menu_CreateNotificationDialog(MemCard_GetActivePort() + 0x47, 0x49);
        D_8009CFFC = (int)MemCard_ClearActivePrompt;
    }
}

void Menu_NavToSaveConfirmDialog(void) {
    MenuWidget_NavScrollTo(0x2A);
}
