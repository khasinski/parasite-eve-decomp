/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef int s32;

typedef struct MenuWidgetNode {
    u8 pad0[0x1C];
    s32 y;
    u8 pad20[0x18];
    s32 selected_index;
    u8 pad3C[0x1C];
    s32 page_count;
    s32 page_start;
    s32 scroll_offset;
    u8 pad64[4];
    s32 has_scroll;
} MenuWidgetNode;

typedef struct MenuWidgetDescriptor {
    s32 pad0;
    s32 y;
} MenuWidgetDescriptor;

MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
MenuWidgetDescriptor *MenuWidget_LookupSimpleDescriptor(int index);
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
void Draw_OffsetCursor(int x, int y);
char *Str_LookupTable4(int index);
void Draw_PrintRawText(char *text);
int Inv_CountTotal(void);
void Draw_PrintNumberWidth2Unk(int value);
void Draw_AllocSprite(int sprite);
int Inv_GetAyaSlotLimit(void);

void Menu_DrawAmmoTypeHeader(MenuWidgetNode *node) {
    register MenuWidgetNode *list asm("$16");
    MenuWidgetDescriptor *desc;
    int page_delta;
    int target_y;

    list = MenuWidget_FindByModeAndSelectedBase(2, 1);
    desc = MenuWidget_LookupSimpleDescriptor(1);
    target_y = (list->selected_index << 4) + 4;
    MenuWidget_OffsetPosition(node, 0, (desc->y + target_y) - node->y);

    if (list->has_scroll != 0) {
        page_delta = (list->page_count - list->selected_index) - list->page_start;
        switch (page_delta) {
        case 0:
            MenuWidget_OffsetPosition(node, 0, list->scroll_offset - 0x10);
            break;
        case 1:
            if (list->scroll_offset < 0) {
                MenuWidget_OffsetPosition(node, 0, list->scroll_offset);
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
