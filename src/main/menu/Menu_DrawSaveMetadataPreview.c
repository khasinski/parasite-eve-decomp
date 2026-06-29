/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK *Save_GetActiveMetadataBuffer();
s32 Save_GetMetadataRemainingChars();
M2C_UNK Sfx_CursorRenderData();
M2C_UNK func_8005E8A4();
M2C_UNK Draw_AllocSprite();
s32 Draw_MeasureTextWidth();
M2C_UNK func_8005F27C();
M2C_UNK Draw_EmitWipeBarRect();
extern void *g_MenuRenameTargetRecord;
extern struct { char _[16]; } D_800C20A4_o __asm__("g_CursorRenderDataBlock");
#define g_CursorRenderDataBlock (*(M2C_UNK *)&D_800C20A4_o)

asm(".globl Menu_DrawSaveMetadataPreview");
asm("Menu_DrawSaveMetadataPreview = func_8004DF74");

void Menu_DrawSaveMetadataPreview(void) __asm__("func_8004DF74");

void Menu_DrawSaveMetadataPreview(void) {
    M2C_UNK *var_a0;
    s32 temp_v0;
    register s32 var_a0_2 asm("$4");
    s32 var_s0;
    volatile s32 frame_pad[2];

    (void) frame_pad;
    if (g_MenuRenameTargetRecord != NULL) {
        func_8005E8A4(0x1C, 0xC);
        Sfx_CursorRenderData(g_MenuRenameTargetRecord);
        var_a0 = &g_CursorRenderDataBlock;
        if (M2C_FIELD(g_MenuRenameTargetRecord, u8 *, 6) == 9) {
            var_a0 = (M2C_UNK *) ((u8 *) &g_CursorRenderDataBlock + 0x10);
        }
        var_a0_2 = Draw_MeasureTextWidth(var_a0) + 0x14;
    } else {
        func_8005E8A4(2, 2);
        Draw_AllocSprite(0x47);
        func_8005E8A4(0x3C, 8);
        func_8005F27C(Save_GetActiveMetadataBuffer());
        var_a0_2 = Draw_MeasureTextWidth(Save_GetActiveMetadataBuffer());
    }
    func_8005E8A4(var_a0_2, 0xC);
    temp_v0 = Save_GetMetadataRemainingChars();
    var_s0 = temp_v0 - 1;
    if (temp_v0 != 0) {
        do {
            Draw_EmitWipeBarRect(9, 4, 0);
            func_8005E8A4(0xB, 0);
            var_s0 -= 1;
        } while (var_s0 != -1);
    }
}
