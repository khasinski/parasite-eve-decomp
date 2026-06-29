/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_OffsetPosition();
M2C_UNK MenuWidget_SetCurrentNode();
void *Inv_LookupData();
M2C_UNK Queue_Init();
void *func_80062D2C();
void *MenuWidget_CreateNode();
M2C_UNK Draw_SetPrimCallback();
M2C_UNK func_80064C20();
extern s32 g_InvItemUsableFlag;
extern void *g_MenuSelectedItemData;
extern struct { char _[16]; } func_8004F644_o __asm__("Menu_DrawEquipStatsPanel");
#define Menu_DrawEquipStatsPanel (*(M2C_UNK *)&func_8004F644_o)
extern struct { char _[16]; } Menu_HandleItemInput_o __asm__("Menu_HandleItemInput");
#define Menu_HandleItemInput (*(M2C_UNK *)&Menu_HandleItemInput_o)
extern struct { char _[16]; } func_8004F798_o __asm__("Menu_DrawEquipItemName");
#define Menu_DrawEquipItemName (*(M2C_UNK *)&func_8004F798_o)
extern struct { char _[16]; } func_8004F7D8_o __asm__("Menu_DrawEquipItemIcon");
#define Menu_DrawEquipItemIcon (*(M2C_UNK *)&func_8004F7D8_o)
extern struct { char _[16]; } func_80050204_o __asm__("Menu_DrawSoundEffectList");
#define Menu_DrawSoundEffectList (*(M2C_UNK *)&func_80050204_o)
extern struct { char _[16]; } func_8005022C_o __asm__("Menu_DrawEquipItemDetailPanel");
#define Menu_DrawEquipItemDetailPanel (*(M2C_UNK *)&func_8005022C_o)

void Menu_CreateItemUsePanel(void) {
    s32 width;
    void *node;
    void *created;
    void *data;
    void *temp_v0;

    temp_v0 = Inv_LookupData();
    g_MenuSelectedItemData = temp_v0;
    if ((u8) M2C_FIELD(temp_v0, u8 *, 6) < 0xAU) {
        node = func_80062D2C(5, 0, 0, 0);
        created = MenuWidget_CreateNode(5, node, node);
        M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawEquipStatsPanel;
        M2C_FIELD(node, M2C_UNK **, 0x2C) = &Menu_HandleItemInput;
        width = 0x80;
        M2C_FIELD(node, s32 *, 0x34) = width;
        MenuWidget_OffsetPosition(node, 0x44, 0x14);
        M2C_FIELD(created, M2C_UNK **, 0x30) = &Menu_DrawSoundEffectList;
        func_80064C20(created);
        M2C_FIELD(created, s32 *, 0x70) = -1;
        MenuWidget_SetCurrentNode(node);
        node = func_80062D2C(6, 0, 0, 0);
        M2C_FIELD(node, s32 *, 0x34) = width;
        MenuWidget_OffsetPosition(node, 0x44, 0x14);
        created = MenuWidget_CreateNode(6, node, node);
        M2C_FIELD(created, M2C_UNK **, 0x30) = &Menu_DrawEquipItemDetailPanel;
        func_80064C20(created);
        data = g_MenuSelectedItemData;
        M2C_FIELD(created, s32 *, 0x3C) = 0x3E;
        Draw_SetPrimCallback(created, M2C_FIELD(data, u8 *, 0x14));
        g_InvItemUsableFlag = M2C_FIELD(g_MenuSelectedItemData, u8 *, 6) != 9;
    } else {
        node = func_80062D2C(0x37, 0, 0, 0);
        M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawEquipItemIcon;
        M2C_FIELD(node, M2C_UNK **, 0x2C) = &Menu_HandleItemInput;
        MenuWidget_SetCurrentNode(node);
    }
    M2C_FIELD(func_80062D2C(0x13, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawEquipItemName;
    Queue_Init();
}
