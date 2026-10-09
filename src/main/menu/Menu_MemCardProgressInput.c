#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/menu_dialog.h"
#include "pe1/text.h"

void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0);
int MemCard_GetActivePort(void);
void Inv_SelectActiveList(int list);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void MemCard_ClearActivePrompt(void);

extern int D_8009CF50;
extern int D_8009CF14;
extern int D_8009CF10;
extern int D_8009CFA4;
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
    MenuWidgetNode *panel;
    MenuWidgetNode *option_node;
    u8 *label;
    int suffix_id;
    MenuConfirmCallback callback;
    int mode;
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
            node = result;
        }
        option_node = MenuWidget_CreateNode(mode, node, node);
        node->draw = Menu_DrawItemLabel;
        panel = node;
        panel->update = Menu_ConfirmDialogHandler;
        option_node->draw = Menu_DrawActionOptionList;
        D_8009CF14 = 0x6C;
        MenuWidget_SaveAndSetCurrentNode(option_node);

        {
            int enabled = 1;
            int active_list;
            suffix_id = 0x4A;
            active_list = D_8009CF10;
            callback = Menu_HandleMemCardWriteOrError;
            option_node->cursor_x = enabled;
            Inv_SelectActiveList(active_list);
        }

        if (label != 0) {
            Util_CopyFFTerminatedBytes(D_800A19C0, label);
        } else {
            D_800A19C0[0] = 0xFF;
        }
        {
            u8 *text_buf;
            Util_AppendFFTerminatedBytes(D_800A19C0, Str_LookupTable4(0x49));
            D_8009CFA4 = suffix_id;
            text_buf = D_800A19C0_measure;
            width = Draw_MeasureTextWidth(text_buf);
            if (width < 0x78) {
                width = 0x78;
            } else {
                text_buf = D_800A19C0_remeasure;
                width = Draw_MeasureTextWidth(text_buf);
            }
        }

        panel->grid_width = width + 0x14;
        panel->visible_rows = 0x42;
        panel->x = (0x12C - width) >> 1;
        option_node->x = (panel->grid_width - 0x80) >> 1;
        height = panel->visible_rows;
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
