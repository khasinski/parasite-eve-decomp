#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
void Draw_OffsetCursor(int x, int y);
char *Str_LookupTable4(int index);
void Draw_PrintRawText(char *text);
int Inv_CountTotal(void);
void Draw_PrintNumberWidth2Unk(int value);
void Draw_AllocSprite(int sprite);
int Inv_GetAyaSlotLimit(void);

void Menu_DrawAmmoTypeHeader(MenuWidgetNode *node) {
    MenuWidgetNode *list;
    MenuWidgetSimpleDescriptor *desc;
    int page_delta;
    int target_y;

    list = MenuWidget_FindByModeAndSelectedBase(2, 1);
    desc = MenuWidget_LookupSimpleDescriptor(1);
    target_y = (list->visible_rows << 4) + 4;
    MenuWidget_OffsetPosition(node, 0, (desc->y + target_y) - node->y);

    if (list->has_scroll != 0) {
        page_delta = (list->y_limit - list->visible_rows) - list->scroll_y;
        switch (page_delta) {
        case 0:
            MenuWidget_OffsetPosition(node, 0, list->scroll_adjust - 0x10);
            break;
        case 1:
            if (list->scroll_adjust < 0) {
                MenuWidget_OffsetPosition(node, 0, list->scroll_adjust);
            }
            break;
        }
    }

    Draw_OffsetCursor(0xA, 2);
    Draw_PrintRawText(Str_LookupTable4(0x25));
    Draw_OffsetCursor(0x46, 4);
    Draw_PrintNumberWidth2Unk(Inv_CountTotal());
    Draw_AllocSprite(0x4C);
    Draw_OffsetCursor(5, 0);
    Draw_PrintNumberWidth2Unk(Inv_GetAyaSlotLimit());
}
