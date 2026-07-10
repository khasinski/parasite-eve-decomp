/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

extern int g_DrawTextPosX;

int Str_LookupTable4(void);
void Draw_PrintCenteredTextInWidth(int arg0, int arg1);

int g_DrawPrimColor;
int g_DrawColorShaded;
int g_TextCursorX;
int g_TextCursorY;
int g_DrawTextPosX;
int D_8009D168;

void Draw_AllocColorTri(int arg0, int arg1, int arg2);

void MenuWidget_DestroyNodeRecursive(void *node);

void MenuWidget_DrawCenteredTableText(void) {
    Draw_PrintCenteredTextInWidth(Str_LookupTable4(), g_DrawTextPosX);
}

void Draw_DropShadow(void) {
    g_DrawPrimColor = 0xFF4000;
    g_DrawColorShaded = 0x7F2000;
    g_TextCursorX -= 2;
    g_TextCursorY -= 2;
    Draw_AllocColorTri(g_DrawTextPosX, D_8009D168, 0);
    g_DrawPrimColor = 0x808080;
    g_DrawColorShaded = 0x404040;
    g_TextCursorX += 2;
    g_TextCursorY += 2;
}

typedef struct MenuPrimNode {
    struct MenuPrimNode *next;
    void *owner4;
    struct MenuPrimNode *children[4];
    int field_18;
    int field_1C;
    int field_20;
    int field_24;
    int field_28;
    void (*step)(void);
    void (*draw)(void);
    void *owner;
    int pad_38;
    int pad_3C;
    int field_40;
} MenuPrimNode;

extern MenuPrimNode *D_8009D154;
extern MenuPrimNode *D_8009D158;

void BoundsCheck_AssertStub(int arg0);
void Draw_FlushFrontBuffer(void);
void Menu_StepListNavigate(void);

void Draw_SwapPrimBuffers(void *owner) {
    register void *owner_reg asm("$17");
    register MenuPrimNode *node asm("$16");
    register MenuPrimNode *parent asm("$18");
    register void *owner4 asm("$19");
    MenuPrimNode *scan;
    int i;

    owner_reg = owner;
    parent = 0;
    scan = D_8009D154;
    while (scan != 0) {
        i = 0;
        while (i < 4) {
            if (scan->children[i] == owner_reg) {
                parent = scan;
                break;
            }
            i++;
        }
        if (i < 4) {
            break;
        }
        scan = scan->next;
    }

    node = D_8009D158;
    owner4 = *(void **)((char *)owner_reg + 4);
    if (node == 0) {
        BoundsCheck_AssertStub(0xA);
    }

    D_8009D158 = node->next;
    node->next = D_8009D154;
    D_8009D154 = node;
    node->owner4 = owner4;
    node->field_28 = 0;
    node->step = 0;

    for (i = 3; i >= 0; i--) {
        node->children[i] = 0;
    }

    node->field_1C = 0;
    node->field_18 = 0;
    node->field_24 = 0;
    node->field_20 = 0;
    node->field_28 = 0;

    if (parent != 0) {
        i = 0;
        while (i < 4) {
            if (parent->children[i] == 0) {
                parent->children[i] = node;
                break;
            }
            i++;
        }
        if (i >= 4) {
            BoundsCheck_AssertStub(0xB);
        }
    }

    if (node == 0) {
        BoundsCheck_AssertStub(0x10);
    }

    node->field_20 = 3;
    node->draw = Draw_FlushFrontBuffer;
    node->step = Menu_StepListNavigate;
    node->owner = owner_reg;
    node->field_40 = *(int *)((char *)owner_reg + 0x64);
    *(MenuPrimNode **)((char *)owner_reg + 0x80) = node;
}

void MenuWidget_DestroyPopupNode(void *node) {
    void *owner;

    owner = *(void **)((char *)node + 0x34);
    *(int *)((char *)owner + 0x80) = 0;
    MenuWidget_DestroyNodeRecursive(node);
}
