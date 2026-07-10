typedef unsigned char u8;

#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define W(base, off) (*(int *)((u8 *)(base) + (off)))

extern int D_8009CF0C;
extern int D_8009CF10;
extern int D_8009CF14;
extern int D_8009CF1C;
extern int D_8009CFA0;
extern int D_8009CFA8;
extern int D_800A1888;
extern int D_800A188C;
extern int D_800A1890;
extern int D_800A1894;
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
    void *child;
    void *parent;
    void *option_node;
    int item;
    int handled;
    int allowed;
    int width;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);

    if ((input & 0x10000) != 0) {
        item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(child));
        if (D_8009CF1C != 0) {
            Inv_SelectActiveList(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 0x36)) == 0);

            allowed = 0;
            if ((D_800A1888 + D_800A188C + D_800A1890 + D_800A1894) != 0) {
                allowed = (U8(Inv_LookupActiveListData(item), 5) & 0x40) == 0;
            }

            if (allowed == 0) {
                Menu_PlayErrorSound();
                return 1;
            }

            Inv_RememberSelection(1, item);
            MenuWidget_NavScrollTo(0x36);
            Menu_ConfigureScreen();
            Menu_PlayConfirmSound();
            return 1;
        }

        if (Inv_IsAyaInventorySlotReserved(item) != 0) {
            Menu_PlayErrorSound();
            return 1;
        }

        if (Menu_GetBattleEquipMode() == 0) {
            Menu_OnEquipConfirm(node, 1);
            Menu_PlayConfirmSound();
            return 1;
        }

        parent = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
        option_node = MenuWidget_CreateNode(0x29, parent, parent);
        W(parent, 0x30) = (int)Menu_DrawItemLabel;
        W(parent, 0x2C) = (int)Menu_ConfirmDialogHandler;
        W(option_node, 0x30) = (int)Menu_DrawActionOptionList;
        D_8009CF14 = 5;
        MenuWidget_SetCurrentNode(option_node);
        Inv_SelectActiveList(D_8009CF10);

        D_800A1980[0] = 0xFF;
        Util_AppendFFTerminatedBytes(D_800A1980, (void *)Str_LookupTable4(0));
        D_8009CFA0 = 0;
        width = Draw_MeasureTextWidth(D_800A1980);
        if (width < 0x78) {
            width = 0x78;
        } else {
            width = Draw_MeasureTextWidth(D_800A1980);
        }

        D_8009CFA8 = (int)Menu_OnEquipConfirm;
        W(parent, 0x34) = width + 0x14;
        W(parent, 0x38) = 0x32;
        W(parent, 0x18) = (0x12C - width) >> 1;
        W(option_node, 0x18) = (W(parent, 0x34) - 0x80) >> 1;
        W(option_node, 0x1C) = W(parent, 0x38) - 0x14;

        Util_CopyFFTerminatedBytes(D_800A1980, (void *)Str_LookupTable4(0x71));
        Util_AppendFFTerminatedBytes(D_800A1980, Inv_LookupActiveListDisplayData(item));
        width = Draw_MeasureTextWidth(D_800A1980);
        if (width < 0x78) {
            width = 0x78;
        } else {
            width = Draw_MeasureTextWidth(D_800A1980);
        }

        parent = MenuWidget_FindByModeAndSelectedBase(1, 0x29);
        W(parent, 0x34) = width + 0x14;
        W(parent, 0x18) = (0x12C - width) >> 1;
        option_node = MenuWidget_FindByModeAndSelectedBase(2, 0x29);
        W(option_node, 0x18) = (W(parent, 0x34) - 0x80) >> 1;
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x1040) == 0) {
        return 0;
    }

    if (D_8009CF0C != 0) {
        MenuWidget_NavScrollTo(0x35);
        child = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
        W(child, 0x44) = -1;
        W(child, 0x5C) = 0;
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 5));
        Menu_PlayCancelSound();
        return 1;
    }

    if ((input & 0x40) == 0) {
        return 1;
    }

    MenuWidget_NavScrollTo(0x35);
    if (D_8009CF1C != 0) {
        MenuWidget_DestroyNode(node);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
        Menu_StepInventoryRoot(0x33E, -1, -1);
        Menu_PlayCancelSound();
        return 1;
    }

    W(child, 0x44) = -1;
    parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
    option_node = MenuWidget_GetCurrentNode();
    if (parent == option_node) {
        W(parent, 0x44) = (W(parent, 0x68) < 1);
        MenuWidget_SetCurrentNode(parent);
    } else {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 5));
    }

    Menu_PlayCancelSound();
    return 1;
}
