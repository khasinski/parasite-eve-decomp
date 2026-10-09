#include "pe1/menu_item_record.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_context_help.h"
#include "pe1/text.h"

extern int g_InvItemUsableFlag;

void Draw_AllocSprite(int);

int Inv_GetPackedListCount(void);
int Inv_GetPackedListItem(int);

void Menu_DrawActionCodeItem(int arg0) {
    int code;

    if (arg0 < g_MenuActionItemData->tailCount) {
        code = g_MenuActionItemData->tailData[arg0] & 0x1F;
        if (code == 0) {
            return;
        }

        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(code + 0x22);
        } else {
            Draw_AllocSprite(code + 0x36);
        }
    } else {
        Draw_AllocSprite(0x46);
    }
}

void Menu_DrawModNameItem(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        Draw_PrintRawText(Str_LookupTable8(Inv_GetPackedListItem(arg0) + 0xEB));
    }
}
