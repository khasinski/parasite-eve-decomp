#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void *MenuWidget_CreateSimpleNode(int mode, int parent, int arg2, int arg3);
M2C_UNK MenuWidget_CreateNode();
void MenuWidget_SetCurrentNode(void *node);
s32 Draw_GetBlendColor();
void Menu_DrawBlendColorChannelListUnk(s32 arg0);
s32 Menu_StepColorSelect(s32 arg0, s32 arg1);
void Menu_DrawBlendColorChannelList(s32 arg0);
void Menu_DrawBlendColorOptionList(s32 arg0);
extern u8 D_800922D4[];
extern s32 D_8009CFE0;

void Menu_OpenBlendColorScreen(s32 arg0) {
    void *temp_v0;
    register void *temp_v0_2 asm("$16");
    void *temp_v0_3;
    void *var_a0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x2E, arg0, 0, 0);
    temp_v0_2 = MenuWidget_CreateNode(0x2E, temp_v0, temp_v0);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x30) = &Menu_DrawBlendColorChannelListUnk;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_StepColorSelect;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x4C) = &D_800922D4;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 1;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0x30) = &Menu_DrawBlendColorChannelList;
    M2C_FIELD(temp_v0_2, s32 *, 0x28) = 1;
    temp_v0_3 = MenuWidget_CreateNode(0x31, temp_v0, temp_v0);
    M2C_FIELD(temp_v0_3, M2C_UNK **, 0x30) = &Menu_DrawBlendColorOptionList;
    M2C_FIELD(temp_v0_2, void **, 0x7C) = temp_v0_3;
    M2C_FIELD(temp_v0_3, void **, 0x78) = temp_v0_2;
    var_a0 = temp_v0_2;
    if (M2C_FIELD(temp_v0_2, s32 *, 0x44) < 0) {
        var_a0 = temp_v0_3;
    }
    MenuWidget_SetCurrentNode(var_a0);
    D_8009CFE0 = Draw_GetBlendColor();
}
#include "common.h"
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuInput_GetStatusFlags();
void Draw_OffsetCursor(int x, int y);
void Draw_SetTextDimmed(int dimmed);
void Draw_AllocSprite(int sprite);
s32 MenuWidget_GetChild();
s32 MenuWidget_GridCellIndex();

void Menu_DrawBlendColorChannelListUnk(s32 arg0) {
    M2C_UNK var_a0;
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    M2C_UNK var_a0_4;
    M2C_UNK var_a0_5;
    M2C_UNK var_a0_6;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s1;

    temp_s1 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
    temp_s0 = MenuInput_GetStatusFlags() & 0x1000;
    asm volatile("" : "=r"(temp_s0) : "0"(temp_s0));
    Draw_OffsetCursor(0x12, 5);
    var_a0 = 0;
    if ((temp_s1 != 0) || (temp_s0 == 0)) {
        var_a0 = 1;
    }
    Draw_SetTextDimmed(var_a0);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    var_a0_2 = 0;
    if ((temp_s1 != 1) || (temp_s0 == 0)) {
        var_a0_2 = 1;
    }
    Draw_SetTextDimmed(var_a0_2);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    var_a0_3 = 0;
    if ((temp_s1 != 2) || (temp_s0 == 0)) {
        var_a0_3 = 1;
    }
    Draw_SetTextDimmed(var_a0_3);
    Draw_AllocSprite(0x4A);
    temp_s0_2 = MenuInput_GetStatusFlags() & 0x4000;
    asm volatile("" : "=r"(temp_s0_2) : "0"(temp_s0_2));
    Draw_OffsetCursor(-0x40, 0x19);
    var_a0_4 = 0;
    if ((temp_s1 != 0) || (temp_s0_2 == 0)) {
        var_a0_4 = 1;
    }
    Draw_SetTextDimmed(var_a0_4);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    var_a0_5 = 0;
    if ((temp_s1 != 1) || (temp_s0_2 == 0)) {
        var_a0_5 = 1;
    }
    Draw_SetTextDimmed(var_a0_5);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    var_a0_6 = 0;
    if ((temp_s1 != 2) || (temp_s0_2 == 0)) {
        var_a0_6 = 1;
    }
    Draw_SetTextDimmed(var_a0_6);
    Draw_AllocSprite(0x4B);
}
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_GridCellIndex();
void MenuWidget_SetCurrentNode(void *node);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayMoveSound(void);
s32 Draw_GetBlendColor();
M2C_UNK Draw_BlendColor();
 s32 MenuWidget_GetChild();
void MenuWidget_DestroyNode(void *node);
extern s32 D_8009CFE0;
extern s32 g_SavedDrawBlendColor[];
#define g_SavedDrawBlendColor (g_SavedDrawBlendColor[0])

s32 Menu_StepColorSelect(s32 arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_a0;
    s32 var_v0;
    s32 clamp;
    s32 var_v0_2;
    void *temp_v0;

    temp_s0 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0)) * 8;
    temp_v1 = Draw_GetBlendColor();
    if (!(arg1 & 0x1000)) {
        goto check4000;
    }
    var_a0 = temp_v1 & ~(0xFF << temp_s0);
    temp_v1_2 = ((temp_v1 >> temp_s0) & 0xFF) + 2;
    if (temp_v1_2 < 0xE9) {
        var_v0 = temp_v1_2 << temp_s0;
        goto blend;
    }
    clamp = 0xE8;
    goto clampshift;
check4000:
    if (!(arg1 & 0x4000)) {
        goto check10000;
    }
    var_a0 = temp_v1 & ~(0xFF << temp_s0);
    temp_v1_3 = ((temp_v1 >> temp_s0) & 0xFF) - 2;
    if (temp_v1_3 >= 0x20) {
        var_v0 = temp_v1_3 << temp_s0;
        goto blend;
    }
    clamp = 0x20;
clampshift:
    var_v0 = clamp << temp_s0;
blend:
    Draw_BlendColor(var_a0 | var_v0);
    Menu_PlayMoveSound();
    return 1;
check10000:
    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetChild(arg0, 1);
        temp_v0_2 = MenuWidget_GridCellIndex(temp_v0);
        if (temp_v0_2 == 0) {
            goto take;
        }
        if (temp_v0_2 != 1) {
            goto setcur;
        }
        Draw_BlendColor(0x404040);
take:
        g_SavedDrawBlendColor = Draw_GetBlendColor();
        MenuWidget_DestroyNode(arg0);
        goto confirm;
setcur:
        M2C_FIELD(temp_v0, s32 *, 0x44) = 0;
        M2C_FIELD(temp_v0, s32 *, 0x48) = 0;
        MenuWidget_SetCurrentNode(temp_v0);
        M2C_FIELD(MenuWidget_GetChild(arg0, 0), s32 *, 0x44) = -1;
confirm:
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Draw_BlendColor(D_8009CFE0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBlendColorChannelRow(int arg0);
void MenuWidget_DrawList(int arg0, void (*arg1)(void));

void Menu_DrawBlendColorChannelList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendColorChannelRow);
}
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void Menu_DrawBlendColorOptionRow(void);
void MenuWidget_DrawList(int arg0, void (*arg1)(void));

int D_8009CFE4;

void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void MenuWidget_SetCurrentNode(void *arg0);
void Menu_DrawScreenAdjustPanel(void);
void Menu_StepScrollList(void);
int Draw_GetBaseY(void);

void Menu_DrawBlendColorOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendColorOptionRow);
}

void Menu_OpenScreenAdjustView(int arg0) {
    void *node;

    node = MenuWidget_CreateSimpleNode(0x38, arg0, 0, 1);
    *(void **)((char *)node + 0x30) = Menu_DrawScreenAdjustPanel;
    *(void **)((char *)node + 0x2C) = Menu_StepScrollList;
    MenuWidget_SetCurrentNode(node);
    D_8009CFE4 = Draw_GetBaseY();
}
void Draw_OffsetCursor(int arg0, int arg1);
int Draw_GetBaseY(void);
void Draw_PrintSignedNumberWidth3(int arg0);
int VSync(int arg0);
void Draw_SetTextDimmed(int arg0);
void Draw_AllocSprite(int arg0);
void Draw_EmitGlyph(int arg0, int arg1);

void Menu_DrawScreenAdjustPanel(void) {
    Draw_OffsetCursor(0x10, 0xA);
    Draw_PrintSignedNumberWidth3(8 - Draw_GetBaseY());
    Draw_SetTextDimmed(VSync(-1) & 0x10);
    Draw_OffsetCursor(-0x10, -0x69);
    Draw_AllocSprite(0x7B);
    Draw_OffsetCursor(0, 0xBE);
    Draw_EmitGlyph(0x7B, 2);
}
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CFE4;

void Draw_SetBaseOffsetPosition(int x, int y);
int Draw_GetBaseY(void);
void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void MenuWidget_DestroyNode(void *node);

int func_8004B650(void *node, int input) {
    int zero_arg;

    if ((input & 0x1000) != 0) {
        zero_arg = 0;
        input = -1;
        goto move;
    }

    if ((input & 0x4000) != 0) {
        zero_arg = 0;
        input = 1;
move:
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    }

    if ((input & 0x10000) != 0) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x40) != 0) {
        Draw_SetBaseOffsetPosition(0, D_8009CFE4 - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}
