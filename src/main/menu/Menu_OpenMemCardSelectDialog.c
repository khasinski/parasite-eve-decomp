/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MemCard_InitState();                        /* extern */
M2C_UNK MemCard_SetDialogActive(M2C_UNK);           /* extern */
void *MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
s32 MenuWidget_GetCurrentNode();                    /* extern */
M2C_UNK MenuWidget_SetCurrentNode(void *);          /* extern */
M2C_UNK Queue_Init();                            /* extern */
void *MenuWidget_CreateSimpleNode(M2C_UNK, s32, M2C_UNK, M2C_UNK) __asm__("func_80062D2C"); /* extern */
void *MenuWidget_CreateNode(M2C_UNK, void *, void *);       /* extern */
extern s32 g_McDialogMode;
extern struct { char _[16]; } Menu_DrawContextHelpText_o __asm__("func_8004C608");
#define Menu_DrawContextHelpText (*(M2C_UNK *)&Menu_DrawContextHelpText_o)
extern struct { char _[16]; } func_8004D2DC_o __asm__("Menu_MemCardPortSelectHandler");
#define Menu_MemCardPortSelectHandler (*(M2C_UNK *)&func_8004D2DC_o)
extern struct { char _[16]; } func_8004FDA4_o __asm__("Menu_IsMemCardSlotSelectable");
#define Menu_IsMemCardSlotSelectable (*(M2C_UNK *)&func_8004FDA4_o)
extern struct { char _[16]; } func_8004FDE8_o __asm__("Menu_DrawMemCardPortList");
#define Menu_DrawMemCardPortList (*(M2C_UNK *)&func_8004FDE8_o)

void Menu_OpenMemCardSelectDialog(s32 arg0) {
    void *temp_a0;
    void *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x24, temp_v0, temp_v0);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_MemCardPortSelectHandler;
    M2C_FIELD(temp_a0, M2C_UNK **, 0x30) = &Menu_DrawMemCardPortList;
    M2C_FIELD(temp_a0, M2C_UNK **, 0x8C) = &Menu_IsMemCardSlotSelectable;
    if (M2C_FIELD(temp_a0, s32 *, 0x48) == 2) {
        M2C_FIELD(temp_a0, s32 *, 0x48) = 0;
    }
    MenuWidget_SetCurrentNode(temp_a0);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        M2C_FIELD(MenuWidget_CreateSimpleNode(0x13, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawContextHelpText;
    }
    M2C_FIELD(MenuWidget_FindByModeAndSelectedBase(1, 0x13), s32 *, 0x38) = 0x24;
    g_McDialogMode = arg0;
    if (arg0 != 0) {
        MemCard_SetDialogActive(1);
        MemCard_InitState();
    }
    Queue_Init();
}
