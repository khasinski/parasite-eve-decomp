/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
s32 MenuWidget_FindLastMode1WithCursorX();          /* extern */
M2C_UNK MenuWidget_SetCurrentNode(void *);          /* extern */
M2C_UNK Queue_Init();                            /* extern */
void *MenuWidget_CreateSimpleNode(s32, void *, M2C_UNK, M2C_UNK); /* extern */
void *MenuWidget_CreateNode(s32, void *, void *);           /* extern */
s32 MenuWidget_GridCellIndex(void *);                          /* extern */
M2C_UNK Draw_SetPrimCallback(void *, M2C_UNK);             /* extern */
extern s32 g_SaveSelectedSlot;
extern struct { char _[16]; } func_8004D690_o __asm__("Menu_DrawSaveSlotPortLabel");
#define Menu_DrawSaveSlotPortLabel (*(M2C_UNK *)&func_8004D690_o)
extern struct { char _[16]; } func_8004D6D4_o __asm__("Menu_StepSaveSelect");
#define Menu_StepSaveSelect (*(M2C_UNK *)&func_8004D6D4_o)
extern struct { char _[16]; } func_8004FE58_o __asm__("Menu_IsMemCardSlotCompatible");
#define Menu_IsMemCardSlotCompatible (*(M2C_UNK *)&func_8004FE58_o)
extern struct { char _[16]; } func_8004FEEC_o __asm__("Menu_DrawSaveSlotList");
#define Menu_DrawSaveSlotList (*(M2C_UNK *)&func_8004FEEC_o)

s32 Menu_CreateSaveSlotListView(s32 arg0, M2C_UNK arg1) {
    s32 temp_s0;
    s32 temp_s0_2;
    void *temp_a0;
    void *temp_s2;
    void *temp_v0;

    temp_s0_2 = arg0 + 0x25;
    temp_v0 = MenuWidget_CreateSimpleNode(temp_s0_2, MenuWidget_FindByModeAndSelectedBase(2, 0x24), 0, 0);
    temp_s2 = MenuWidget_CreateNode(temp_s0_2, temp_v0, temp_v0);
    temp_a0 = temp_s2;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_StepSaveSelect;
    M2C_FIELD(temp_s2, M2C_UNK **, 0x30) = &Menu_DrawSaveSlotList;
    M2C_FIELD(temp_s2, M2C_UNK **, 0x8C) = &Menu_IsMemCardSlotCompatible;
    Draw_SetPrimCallback(temp_a0, arg1);
    temp_s0 = MenuWidget_GridCellIndex(temp_s2);
    if (MenuWidget_FindLastMode1WithCursorX() == 0) {
        M2C_FIELD(temp_s2, s32 *, 0x44) = 0;
        MenuWidget_SetCurrentNode(temp_s2);
    } else {
        M2C_FIELD(temp_s2, s32 *, 0x44) = -1;
    }
    M2C_FIELD(MenuWidget_CreateSimpleNode(0x3F, temp_s2, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawSaveSlotPortLabel;
    Queue_Init();
    g_SaveSelectedSlot = arg0;
    return temp_s0;
}
