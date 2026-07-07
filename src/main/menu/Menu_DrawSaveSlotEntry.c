/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MemCard_GetSlot();
s32 Save_GetMetadataWindowIndex();
void *func_8005DD8C();
M2C_UNK Draw_OffsetCursor();
M2C_UNK Draw_AllocSprite();
M2C_UNK Draw_PrintRawText();
M2C_UNK func_8005F5B8();
M2C_UNK Draw_PrintNumberWidth2Unk();
M2C_UNK Draw_PrintNumberWidth3Unk();
M2C_UNK Draw_PrintNumberWidth4Unk();
M2C_UNK Draw_PrintTimeValue();
M2C_UNK func_800605F8();
M2C_UNK func_800614AC();
M2C_UNK Draw_EmitWipeBarRect();
M2C_UNK Menu_SetSaveSlotBlendBase();
s32 MenuWidget_GridCellIndex();
M2C_UNK func_80064C54();
extern void *g_MenuActiveWidget;
extern s32 g_SavedDrawBlendColor[];
#define g_SavedDrawBlendColor (g_SavedDrawBlendColor[0])

void Menu_DrawSaveSlotEntry(s32 arg0) {
    register void *slot;
    s32 index;
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    M2C_UNK var_a1;
    u8 state;

    index = arg0;
    slot = MemCard_GetSlot(M2C_FIELD(g_MenuActiveWidget, s32 *, 0x24) - 0x25, index);
    if (slot != NULL) {
        state = M2C_FIELD(slot, u8 *, 0);
        switch (state) {
        case 1:
            if (index == MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
                Menu_SetSaveSlotBlendBase(M2C_FIELD(slot, s32 *, 0x20));
            } else {
                Draw_OffsetCursor(-2, -2);
                func_800614AC(M2C_FIELD(slot, s32 *, 0x20));
                Draw_EmitWipeBarRect(M2C_FIELD(g_MenuActiveWidget, s32 *, 0x3C), M2C_FIELD(g_MenuActiveWidget, s32 *, 0x40), 1);
                func_800614AC(g_SavedDrawBlendColor);
                Draw_OffsetCursor(2, 2);
            }
            Draw_OffsetCursor(2, 2);
            if (Save_GetMetadataWindowIndex() == 0) {
                Draw_PrintRawText(slot + 4);
            } else {
                Draw_PrintRawText(slot + 0x14);
            }
            Draw_OffsetCursor(0x58, 0);
            if (M2C_FIELD(slot, u8 *, 0x2A) != 0) {
                Draw_AllocSprite(0x95);
                Draw_OffsetCursor(0x1A, 2);
                Draw_AllocSprite(0x96);
                Draw_OffsetCursor(0x22, 1);
                Draw_PrintNumberWidth2Unk(M2C_FIELD(slot, u8 *, 0x2A) + 1);
                var_a0_2 = 4;
                var_a1 = -3;
            } else {
                var_a0_2 = 0x4A;
                var_a1 = 0;
            }
            Draw_OffsetCursor(var_a0_2, var_a1);
            func_8005F5B8(0x5A);
            Draw_OffsetCursor(0x19, 0);
            func_800605F8(index + 1);
            Draw_OffsetCursor(-0xC4, 0xF);
            Draw_AllocSprite(0x53);
            Draw_OffsetCursor(0x1E, 1);
            Draw_PrintNumberWidth3Unk(M2C_FIELD(slot, u8 *, 0x28) + 1);
            Draw_OffsetCursor(8, -1);
            Draw_AllocSprite(0x54);
            Draw_OffsetCursor(0x10, 1);
            Draw_PrintNumberWidth4Unk(M2C_FIELD(slot, s16 *, 0x24));
            Draw_AllocSprite(0x4C);
            Draw_OffsetCursor(5, 0);
            Draw_PrintNumberWidth4Unk(M2C_FIELD(slot, s16 *, 0x26));
            Draw_OffsetCursor(8, -1);
            Draw_AllocSprite(0x55);
            Draw_OffsetCursor(0x1A, 1);
            Draw_PrintTimeValue(M2C_FIELD(slot, s32 *, 0xC), 0);
            Draw_OffsetCursor(-0xC0, 0xC);
            if (M2C_FIELD(slot, u8 *, 0x29) != 0) {
                func_8005F5B8(0x60);
                return;
            }
            func_8005F5B8(0xB);
            Draw_OffsetCursor(0x1E, 0);
            func_800605F8(M2C_FIELD(slot, s16 *, 0x2C));
            Draw_OffsetCursor(0x14, 0);
            Draw_PrintRawText(func_8005DD8C(M2C_FIELD(slot, s16 *, 0x2E)));
            return;
        case 2:
            Draw_OffsetCursor(0, 0x12);
            var_a0_3 = 0x41;
            goto block_20;
        case 3:
            Draw_OffsetCursor(0, 0x12);
            var_a0_3 = 0x76;
block_20:
            func_80064C54(var_a0_3);
            Draw_OffsetCursor(0x5C, -0x10);
            break;
        }
    }
}
