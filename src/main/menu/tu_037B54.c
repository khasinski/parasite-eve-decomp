/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuInventoryViewMode;

int MenuWidget_GetChild(int arg0, int arg1);
int MenuWidget_GridCellIndex(int arg0);
void Sort_ContainerItems(int arg0);
void Inv_SortInventoryByMode(int arg0, int arg1);
void Menu_OpenInventoryOrSwapView(int arg0);
void Menu_PlayConfirmSound(void);
void MenuWidget_DestroyNode(int arg0);
void Menu_PlayCancelSound(void);

int g_InvItemUsableFlag;
int g_MenuInventoryViewMode;

void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void *MenuWidget_CreateNode(int arg0, void *arg1, void *arg2);
void MenuWidget_SetCurrentNode(void *arg0);
void MenuWidget_OffsetPosition(void *arg0, int arg1, int arg2);
void Menu_EquipSelectInput(void);
void Menu_DrawEquipInfoOptionList(void);

int Menu_EquipGridHandler(int arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (g_MenuInventoryViewMode != 0) {
            Sort_ContainerItems(temp);
        } else {
            Inv_SortInventoryByMode(2, temp);
        }
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Menu_CreateEquipInfoPanel(int arg0, unsigned int arg1) {
    void *node;
    void *child;

    node = MenuWidget_CreateSimpleNode(0x3C, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x3C, node, node);
    *(void **)((char *)node + 0x2C) = Menu_EquipSelectInput;
    *(void **)((char *)child + 0x30) = Menu_DrawEquipInfoOptionList;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuInventoryViewMode == 0) {
        MenuWidget_OffsetPosition(
            node,
            0x70 - *(int *)((char *)node + 0x18),
            0x20 - *(int *)((char *)node + 0x1C)
        );
    }

    g_InvItemUsableFlag = arg1 < 1;
    if (arg1 != 0) {
        MenuWidget_OffsetPosition(node, 0, 0x14);
    }
}
