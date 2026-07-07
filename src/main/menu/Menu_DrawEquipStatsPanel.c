/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Menu_DrawEquipStatsDelta(void *);                      /* extern */
M2C_UNK Draw_OffsetCursor(M2C_UNK, M2C_UNK);            /* extern */
M2C_UNK Draw_AllocSprite(M2C_UNK);                     /* extern */
M2C_UNK Draw_PrintNumberWidth4Unk(u8);                          /* extern */
M2C_UNK Draw_PrintSignedNumberWidth4(s16);                         /* extern */
extern void *g_MenuSelectedItemData;

void Menu_DrawEquipStatsPanel(void) {
    void *temp_s0;

    Menu_DrawEquipStatsDelta(g_MenuSelectedItemData);
    temp_s0 = g_MenuSelectedItemData;
    if (temp_s0 != NULL) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(M2C_FIELD(temp_s0, u8 *, 9));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(M2C_FIELD(temp_s0, s16 *, 0x12));
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(M2C_FIELD(temp_s0, u8 *, 8));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(M2C_FIELD(temp_s0, s16 *, 0x10));
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(M2C_FIELD(temp_s0, u8 *, 7));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(M2C_FIELD(temp_s0, s16 *, 0xE));
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}
