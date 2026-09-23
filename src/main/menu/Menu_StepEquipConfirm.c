/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define W(base, off) (*(int *)((u8 *)(base) + (off)))

extern int D_8009CF0C;
extern int D_8009CF10;
extern int D_8009CF14;
extern int D_8009CF1C;
extern int D_8009CFA0;
extern int D_8009CFA8;
extern int D_800A1888[];
extern int D_800A188C[];
extern int D_800A1890[];
extern int D_800A1894[];
extern u8 D_800A1980[];

void *MenuWidget_GetChild(void *node, int index);
int MenuWidget_GridCellIndex(void *node);
int Inv_GetPackedListItem(int index);
void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void Inv_SelectActiveList(int use_override);
void *Inv_LookupActiveListData(int index);
void Inv_RememberSelection(int side, int item);
int Inv_IsAyaInventorySlotReserved(int slot);
int Menu_GetBattleEquipMode(void);
void *MenuWidget_CreateSimpleNode(int mode, void *parent, int arg2, int arg3);
void *MenuWidget_CreateNode(int mode, void *parent, void *selected_base);
void Menu_DrawItemLabel(void);
void Menu_ConfirmDialogHandler(void);
void Menu_DrawActionOptionList(void);
void MenuWidget_SetCurrentNode(void *node);
int Str_LookupTable4(int index);
void Util_CopyFFTerminatedBytes(void *dst, void *src);
void Util_AppendFFTerminatedBytes(void *dst, void *src);
int Draw_MeasureTextWidth(void *text);
void *Inv_LookupActiveListDisplayData(int index);
void Menu_OnEquipConfirm(void *node, int confirmed);
void Menu_PlayConfirmSound(void);
void Menu_PlayErrorSound(void);
void Menu_PlayCancelSound(void);
void MenuWidget_NavScrollTo(int selected_base);
void MenuWidget_DestroyNode(void *node);
void *MenuWidget_GetCurrentNode(void);
void Menu_StepInventoryRoot(int flags, int arg1, int arg2);
void Menu_ConfigureScreen(void);

int Menu_StepEquipConfirm(void *node, int input) {
    register void *child asm("$16");
    void *parent;
    void *option_node;
    int item;
    register int handled asm("$18");
    int allowed;
    int first_width;
    int width;
    register void *second_parent asm("$19");
    register u8 *dialog_text asm("$17");
    void *callback;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);

    if ((input & 0x10000) == 0) goto no_confirm;
    {
        item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(child));
        if (D_8009CF1C != 0) {
            Inv_SelectActiveList(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 0x36)) == 1);

            allowed = 0;
            if ((D_800A1888[0] + D_800A188C[0] + D_800A1890[0] + D_800A1894[0]) != 0) {
                {
                    unsigned mask = U8(Inv_LookupActiveListData(item), 5) & 0x40;
                    /* Preserve the retail AND/SLTIU sequence. */
                    asm volatile("" : : "r"(mask));
                    allowed = mask < 1U;
                }
            }

            if (allowed == 0) {
                goto error;
            }

            Inv_RememberSelection(1, item);
            MenuWidget_NavScrollTo(0x36);
            Menu_ConfigureScreen();
            goto confirm;
        }

        goto reserved;
confirm:
        handled = 1;
        Menu_PlayConfirmSound();
        goto done;
reserved:
        if (Inv_IsAyaInventorySlotReserved(item) != 0) {
            goto error;
        }

        if (Menu_GetBattleEquipMode() == 0) goto simple_confirm;

        {
            void *result = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
            register int mode asm("$4") = 0x29;
            /* Keep the mode argument ready before copying the returned node. */
            asm volatile("" : : "r"(mode));
            parent = result;
            option_node = MenuWidget_CreateNode(mode, parent, parent);
        }
        W(parent, 0x30) = (int)Menu_DrawItemLabel;
        W(parent, 0x2C) = (int)Menu_ConfirmDialogHandler;
        W(option_node, 0x30) = (int)Menu_DrawActionOptionList;
        D_8009CF14 = 5;
        MenuWidget_SetCurrentNode(option_node);
        Inv_SelectActiveList(D_8009CF10);

        dialog_text = D_800A1980;
        dialog_text[0] = 0xFF;
        Util_AppendFFTerminatedBytes(dialog_text, (void *)Str_LookupTable4(0));
        D_8009CFA0 = 0;
        callback = Menu_OnEquipConfirm;
        first_width = Draw_MeasureTextWidth(dialog_text);
        if (first_width < 0x78) {
            first_width = 0x78;
        } else {
            first_width = Draw_MeasureTextWidth(dialog_text);
        }

        W(parent, 0x34) = first_width + 0x14;
        W(parent, 0x38) = 0x32;
        W(parent, 0x18) = (0x12C - first_width) >> 1;
        W(option_node, 0x18) = (W(parent, 0x34) - 0x80) >> 1;
        {
            int height = W(parent, 0x38);
            D_8009CFA8 = (int)callback;
            W(option_node, 0x1C) = height - 0x14;
        }

        Util_CopyFFTerminatedBytes(D_800A1980, (void *)Str_LookupTable4(0x71));
        Util_AppendFFTerminatedBytes(D_800A1980, Inv_LookupActiveListDisplayData(item));
        if (Draw_MeasureTextWidth(D_800A1980) >= 0x78) {
            width = Draw_MeasureTextWidth(D_800A1980);
        } else {
            width = 0x78;
        }

        second_parent = MenuWidget_FindByModeAndSelectedBase(1, 0x29);
        /* Copy the returned node before preparing the next lookup. */
        asm volatile("" : : "r"(second_parent));
        W(second_parent, 0x34) = width + 0x14;
        W(second_parent, 0x18) = (0x12C - width) >> 1;
        option_node = MenuWidget_FindByModeAndSelectedBase(2, 0x29);
        W(option_node, 0x18) = (W(second_parent, 0x34) - 0x80) >> 1;
        goto confirm;
    }
simple_confirm:
    Menu_OnEquipConfirm(node, 1);
    goto confirm;
error:
    handled = 1;
    Menu_PlayErrorSound();
    goto done;
no_confirm:
    if ((input & 0x1040) == 0) {
        goto done;
    }

    if (D_8009CF0C != 0) {
        MenuWidget_NavScrollTo(0x35);
        W(child, 0x44) = -1;
        W(child, 0x5C) = 0;
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x36));
        goto cancel;
    }

    if ((input & 0x40) == 0) {
        handled = 1;
        goto done;
    }

    MenuWidget_NavScrollTo(0x35);
    if (D_8009CF1C != 0) {
        MenuWidget_DestroyNode(node);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
        Menu_StepInventoryRoot(0x33E, -1, -1);
        goto cancel;
    }

    W(child, 0x44) = -1;
    parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
    option_node = MenuWidget_GetCurrentNode();
    if (parent == option_node) {
        W(parent, 0x44) = ((unsigned)W(parent, 0x68) < 1U);
        MenuWidget_SetCurrentNode(parent);
    } else {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 5));
    }

    goto cancel;

cancel:
    handled = 1;
    Menu_PlayCancelSound();
done:
    return handled;
}
