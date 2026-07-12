/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

s32 MenuWidget_GetChild(s32 node, s32 index);
s32 MenuWidget_GetCellIndex(s32 node);
s32 MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void MenuWidget_DestroyNode(s32 node);
void Menu_CreateBonusPointAllocationView(void);
void Menu_StepItemDetailPanel2(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayErrorSound(void);
int MenuWidget_GridCellIndex(int node);
int Inv_GetPackedListItem(int index);
int MenuWidget_GetCurrentNode(void);
int Str_LookupTable8(int index);
int MenuWidget_CreateSimpleNode(int kind, int parent, int arg2, int arg3);
int MenuWidget_CreateNode(int kind, int parent, int selected_base);
void Menu_DrawItemLabel(void);
void Menu_ConfirmDialogHandler(void);
void Menu_DrawActionOptionList(void);
void MenuWidget_SetCurrentNode(int node);
void Inv_SelectActiveList(int list);
void Util_CopyFFTerminatedBytes(int dst, int src);
int Str_LookupTable4(int index);
void Util_AppendFFTerminatedBytes(int dst, int src);
int Draw_MeasureTextWidth(int str);

int g_MenuBattleSelectedItem;
extern int D_8009CFB4;
extern int D_8009CF14;
extern int D_8009CF10;
extern int D_8009CFA0;
extern int D_8009CFA8;
extern unsigned char D_800A1980[];

int Menu_GetBattleEquipMode(void);
void MenuWidget_NavScrollTo(int arg0);
void Battle_UseItem(int arg0);
void Menu_OnBattleCommandConfirm(int arg0, int arg1);

s32 Menu_SkillActionHandler(s32 node, s32 flags) {
    s32 child;
    s32 handled;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        if (MenuWidget_GetCellIndex(child) >= 0) {
            handled = 1;
            Menu_StepItemDetailPanel2();
            Menu_PlayConfirmSound();
        } else {
            handled = 1;
            Menu_PlayErrorSound();
        }
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(node);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1D));
        Menu_CreateBonusPointAllocationView();
        handled = 1;
        Menu_PlayCancelSound();
    }
    return handled;
}

void Menu_StepItemDetailPanel2(void) {
    int node;
    int current;
    register int label asm("$18");
    register int option_node asm("$17");
    int width;
    register int callback asm("$19");
    int height;

    D_8009CFB4 = Inv_GetPackedListItem(MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 8)));
    current = MenuWidget_GetCurrentNode();
    label = Str_LookupTable8(D_8009CFB4 + 0xEB);
    node = MenuWidget_CreateSimpleNode(0x29, current, 0, 1);
    option_node = MenuWidget_CreateNode(0x29, node, node);
    *(int *)(node + 0x30) = (int)Menu_DrawItemLabel;
    *(int *)(node + 0x2C) = (int)Menu_ConfirmDialogHandler;
    *(int *)(option_node + 0x30) = (int)Menu_DrawActionOptionList;
    D_8009CF14 = 5;
    MenuWidget_SetCurrentNode(option_node);
    callback = (int)Menu_OnBattleCommandConfirm;
    Inv_SelectActiveList(D_8009CF10);

    if (label != 0) {
        Util_CopyFFTerminatedBytes((int)D_800A1980, label);
    } else {
        D_800A1980[0] = 0xFF;
    }

    {
        int suffix;
        register int text_buf asm("$4");

        suffix = Str_LookupTable4(0x1A);
        asm volatile(
            "lui %0, %%hi(D_800A1980)\n"
            "addiu %0, %0, %%lo(D_800A1980)"
            : "=r"(text_buf));
        Util_AppendFFTerminatedBytes(text_buf, suffix);
        D_8009CFA0 = 0;
        asm volatile(
            "lui %0, %%hi(D_800A1980)\n"
            "addiu %0, %0, %%lo(D_800A1980)"
            : "=r"(text_buf));
        width = Draw_MeasureTextWidth(text_buf);
        if (width < 0x78) {
            width = 0x78;
        } else {
            asm volatile(
                "lui %0, %%hi(D_800A1980)\n"
                "addiu %0, %0, %%lo(D_800A1980)"
                : "=r"(text_buf));
            width = Draw_MeasureTextWidth(text_buf);
        }
    }

    *(int *)(node + 0x34) = width + 0x14;
    *(int *)(node + 0x38) = 0x32;
    *(int *)(node + 0x18) = (0x12C - width) >> 1;
    *(int *)(option_node + 0x18) = (*(int *)(node + 0x34) - 0x80) >> 1;
    height = *(int *)(node + 0x38);
    D_8009CFA8 = callback;
    *(int *)(option_node + 0x1C) = height - 0x14;
}

void Menu_OnBattleCommandConfirm(int arg0, int arg1) {
    if (arg1 != 0) {
        if (Menu_GetBattleEquipMode() != 0) {
            MenuWidget_NavScrollTo(8);
        }
        Battle_UseItem(g_MenuBattleSelectedItem);
    }
}
