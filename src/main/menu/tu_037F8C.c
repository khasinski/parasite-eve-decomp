/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuSelectedItemIndex;
int D_8009CFBC;

int MenuWidget_GetChild(int arg0, int arg1);
int MenuWidget_GetCellIndex(int arg0);
void Menu_CreateItemActionSubmenu(void);
void Menu_CreateItemList(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayErrorSound(void);
void MenuWidget_DestroyNode(int arg0);
void Menu_PlayCancelSound(void);

int g_MenuActionTextBase;
int g_MenuSelectedItemIndex;

void *MenuWidget_GetCurrentNode(void);
void *MenuWidget_CreateSimpleNode(int arg0, void *arg1, int arg2, int arg3);
void *MenuWidget_CreateNode(int arg0, void *arg1, void *arg2);
void MenuWidget_SetCurrentNode(void *arg0);
void Menu_DrawItemActionSubmenu(void);
void Menu_StepSkillSelect(void);
void Menu_DrawActionOptionList(void);

typedef unsigned char u8;

int g_InvItemUsableFlag;
int g_MenuSelectedItemList;
int g_MenuSelectedItemSlot;
int g_MenuSelectedItemIndex;

int Inv_RestoreSelection(int arg0);
void Draw_OffsetCursor(int arg0, int arg1);
void Draw_StatePush(void);
u8 *Inv_LookupActiveListData(int arg0);
void Draw_AllocSprite(int arg0);
void Draw_PrintTextById(int arg0);
int Str_LookupTable4(int arg0);
int Draw_MeasureTextWidth(int arg0);
void Draw_StatePop(void);
void Sfx_DrawActiveListSlot(int arg0);

int Menu_InventoryItemHandler(int arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GetCellIndex(MenuWidget_GetChild(arg0, 0));
        g_MenuSelectedItemIndex = temp;
        if (temp >= 0) {
            if ((temp & 1) || (D_8009CFBC == 0)) {
                Menu_CreateItemActionSubmenu();
            } else {
                Menu_CreateItemList();
            }
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Menu_CreateItemActionSubmenu(void) {
    void *node;
    void *child;

    node = MenuWidget_CreateSimpleNode(4, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(4, node, node);
    *(void **)((char *)node + 0x30) = Menu_DrawItemActionSubmenu;
    *(void **)((char *)node + 0x2C) = Menu_StepSkillSelect;
    *(void **)((char *)child + 0x30) = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuSelectedItemIndex & 1) {
        *(int *)((char *)node + 0x38) -= 0x1C;
        *(int *)((char *)child + 0x1C) -= 0x1C;
    }
}

void Menu_DrawItemActionSubmenu(void) {
    int selected;
    int icon;
    u8 *entry;

    selected = Inv_RestoreSelection(g_MenuSelectedItemList);
    Draw_OffsetCursor(4, 4);
    Draw_StatePush();

    if (g_MenuSelectedItemSlot >= 0) {
        entry = Inv_LookupActiveListData(selected);
        entry += g_MenuSelectedItemSlot;
        icon = entry[0x15] & 0x1F;
        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(icon + 0x22);
        } else {
            Draw_AllocSprite(icon + 0x36);
        }
        Draw_OffsetCursor(0x24, 0);
    } else {
        Draw_PrintTextById(0x66);
        Draw_OffsetCursor(Draw_MeasureTextWidth(Str_LookupTable4(0x66)), 0);
    }

    Draw_PrintTextById((g_MenuSelectedItemIndex & 1) | 0x64);
    Draw_StatePop();

    if ((g_MenuSelectedItemIndex & 1) == 0) {
        Draw_OffsetCursor(0, 0xE);
        Sfx_DrawActiveListSlot(selected);
        Draw_OffsetCursor(0, 0xE);
        Draw_PrintTextById(0x67);
    }

    Draw_OffsetCursor(0, 0x14);
    Draw_PrintTextById(0x68);
}
