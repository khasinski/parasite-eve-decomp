/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

void MenuWidget_NavScrollTo(int selected_base);
int MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
int MenuWidget_CreateSimpleNode(int kind, int parent, int arg2, int arg3);
int MenuWidget_CreateNode(int kind, int parent, int sibling);
void MenuWidget_SaveAndSetCurrentNode(int node);
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

int Menu_MemCardProgressInputHandler(void) {
    return 1;
}

void Menu_StepItemGrid2(void) {
    register int node asm("$16");
    register int option_node asm("$17");
    register int label asm("$18");
    register int suffix_id asm("$19");
    register int callback asm("$20");
    register int mode asm("$4");
    int width;
    int height;

    if (D_8009CF50 != 0) {
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x2A) != 0) {
            return;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, 0x24);
        label = Str_LookupTable4(MemCard_GetActivePort() + 0x47);
        node = MenuWidget_CreateSimpleNode(0x2A, node, 0, 1);
        asm volatile(
            "addiu %1,$0,0x2A\n\t"
            "addu %0,$2,$0"
            : "=r"(node), "=r"(mode));
        option_node = MenuWidget_CreateNode(mode, node, node);
        *(void **)(node + 0x30) = Menu_DrawItemLabel;
        *(void **)(node + 0x2C) = Menu_ConfirmDialogHandler;
        *(void **)(option_node + 0x30) = Menu_DrawActionOptionList;
        D_8009CF14 = 0x6C;
        MenuWidget_SaveAndSetCurrentNode(option_node);

        asm volatile(
            ".set push\n\t"
            ".set noreorder\n\t"
            "addiu $2,$0,1\n\t"
            "addiu %1,$0,0x4A\n\t"
            ".word 0x8F840000\n\t"
            ".reloc .-4, R_MIPS_GPREL16, D_8009CF10\n\t"
            "lui %0,%%hi(Menu_HandleMemCardWriteOrError)\n\t"
            "addiu %0,%0,%%lo(Menu_HandleMemCardWriteOrError)\n\t"
            ".word 0x0C000000\n\t"
            ".reloc .-4, R_MIPS_26, Inv_SelectActiveList\n\t"
            "sw $2,0x44(%2)\n\t"
            ".set pop"
            : "=r"(callback), "=r"(suffix_id)
            : "r"(option_node)
            : "$2", "$4", "$31", "memory");

        if (label != 0) {
            Util_CopyFFTerminatedBytes((int)D_800A19C0, label);
        } else {
            D_800A19C0[0] = 0xFF;
        }
        {
            register int text_buf asm("$4");

            Util_AppendFFTerminatedBytes((int)D_800A19C0, Str_LookupTable4(0x49));
            D_8009CFA4 = suffix_id;
            asm volatile(
                "lui %0, %%hi(D_800A19C0)\n"
                "addiu %0, %0, %%lo(D_800A19C0)"
                : "=r"(text_buf));
            width = Draw_MeasureTextWidth(text_buf);
            if (width < 0x78) {
                width = 0x78;
            } else {
                asm volatile(
                    "lui %0, %%hi(D_800A19C0)\n"
                    "addiu %0, %0, %%lo(D_800A19C0)"
                    : "=r"(text_buf));
                width = Draw_MeasureTextWidth(text_buf);
            }
        }

        *(int *)(node + 0x34) = width + 0x14;
        *(int *)(node + 0x38) = 0x42;
        *(int *)(node + 0x18) = (0x12C - width) >> 1;
        *(int *)(option_node + 0x18) = (*(int *)(node + 0x34) - 0x80) >> 1;
        height = *(int *)(node + 0x38);
        D_8009CFA8 = callback;
        *(int *)(option_node + 0x1C) = height - 0x14;
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
