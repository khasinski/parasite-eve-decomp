/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
s32 MenuWidget_GetCurrentNode();                    /* extern */
M2C_UNK MenuWidget_SetCurrentNode(void *);          /* extern */
M2C_UNK MenuWidget_SetCursorY(s32);                 /* extern */
void *MenuWidget_CreateSimpleNode(M2C_UNK, s32, M2C_UNK, M2C_UNK); /* extern */
void *MenuWidget_CreateNode(M2C_UNK, void *, void *);       /* extern */
extern M2C_UNK Menu_DrawSkillSelectionList[];
#define Menu_DrawSkillSelectionList (Menu_DrawSkillSelectionList[0])
extern M2C_UNK Menu_HandleAyaInventorySelectionInput[];
#define Menu_HandleAyaInventorySelectionInput (Menu_HandleAyaInventorySelectionInput[0])

void Menu_OpenSkillSelectionView(void) {
    void *temp_a0;
    void *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x40, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x40, temp_v0, temp_v0);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_HandleAyaInventorySelectionInput;
    M2C_FIELD(temp_a0, M2C_UNK **, 0x30) = &Menu_DrawSkillSelectionList;
    MenuWidget_SetCurrentNode(temp_a0);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
}
