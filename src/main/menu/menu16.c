#include "pe1/menu_widget.h"

void MenuWidget_NavScrollTo(int selected_base);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);

extern char g_MenuTwoLineDialogText[];
extern char D_800A1A20[];

void Draw_OffsetCursor(int x, int y);
void Draw_PrintCenteredText(int arg0);
int Draw_MeasureTextWidth(char *text);
void Menu_DrawTwoLineDialogText(void);
void Menu_SetDeferredCallback(void (*callback)(void));
int Menu_HandleDeferredCallbackInput(int arg0, int arg1);
void Menu_DrawNotificationDialogContent(void);
char *Str_LookupTable4(int id);
void Util_CopyFFTerminatedBytes(char *dst, char *src);
void Queue_Init(void);

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}

void Menu_DrawNotificationText(int *arg0) {
    char *text;

    Draw_OffsetCursor(0, 0xA);
    text = g_MenuTwoLineDialogText;
    if (arg0[9] == 0x3D) {
        text += 0x40;
    }
    Draw_PrintCenteredText(text);
}

void Menu_CreateTwoLineDialog(int line0_id, int line1_id) {
    register MenuWidgetNode *parent asm("$18");
    register MenuWidgetNode *child asm("$20");
    register char *line0 asm("$19");
    register char *line1 asm("$17");
    char *text;
    register int width asm("$3");

    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->field_30 = Menu_DrawTwoLineDialogText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->field_30 = Menu_DrawNotificationDialogContent;

    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);

    text = Str_LookupTable4(line0_id);
    line0 = D_800A1A20;
    Util_CopyFFTerminatedBytes(line0, text);

    text = Str_LookupTable4(line1_id);
    line1 = line0 + 0x40;
    Util_CopyFFTerminatedBytes(line1, text);

    /* GCC folds the width tests into a different CFG; keep this picker fixed. */
    asm volatile(
        ".set push\n\t"
        ".set noreorder\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "addu $4,%1,$0\n\t"
        "addu $4,%2,$0\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "addu $16,$2,$0\n\t"
        "slt $2,$2,$16\n\t"
        ".word 0x10400008\n\t"
        "nop\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "addu $4,%1,$0\n\t"
        "slti $2,$2,0x64\n\t"
        ".word 0x10400008\n\t"
        "addiu $3,$0,0x64\n\t"
        ".word 0x08000000\n\t"
        ".reloc .-4, R_MIPS_26, 3f\n\t"
        "nop\n"
        "1:\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "addu $4,%2,$0\n\t"
        "slti $2,$2,0x64\n\t"
        ".word 0x14400010\n\t"
        "addiu $3,$0,0x64\n"
        "2:\n\t"
        "lui $17,%%hi(D_800A1A20)\n\t"
        "addiu $17,$17,%%lo(D_800A1A20)\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "addu $4,$17,$0\n\t"
        "addiu $19,$17,0x40\n\t"
        "addu $4,$19,$0\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "addu $16,$2,$0\n\t"
        "slt $2,$2,$16\n\t"
        ".word 0x14400002\n\t"
        "addu $4,$17,$0\n\t"
        "addu $4,$19,$0\n"
        "4:\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, Draw_MeasureTextWidth\n\t"
        "nop\n\t"
        "addu $3,$2,$0\n"
        "3:\n\t"
        ".set pop"
        : "=r"(width)
        : "r"(line0), "r"(line1)
        : "$2", "$4", "$16", "$17", "$19", "$31", "memory");

    parent->grid_width = width + 0x14;
    parent->value_18 = (0x12C - width) >> 1;
    parent->visible_rows += 0xE;
    child->value_18 = parent->grid_width - 0x44;
    child->value_1C += 0xE;

    Queue_Init();
}

void Menu_DrawTwoLineDialogText(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText);
    Draw_OffsetCursor(0, 0xE);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText + 0x40);
}
