/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_widget.h"
#include "pe1/inventory.h"

extern int D_8009CF0C;
extern int D_800A1888[], D_800A188C[], D_800A1890[], D_800A1894[];
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int, int);
int MenuWidget_IsCursorYClear(MenuWidgetNode *);
int MenuWidget_GridCellIndex(MenuWidgetNode *);
int Inv_GetPackedListItem(int);
int Inv_RestoreSelection(unsigned int);
int Inv_GetPackedListCount(void);
void MenuWidget_OffsetPosition(MenuWidgetNode *, int, int);
void Draw_OffsetCursor(int, int);
void Draw_PrintTextById(int);
void Draw_PrintNumberWidth2Unk(int);
void Draw_PrintNumberWidth3Unk(int);

void Menu_DrawItemDetailPanel(MenuWidgetNode *panel) {
    MenuWidgetNode *selection;
    ItemDataRecord *item;
    int index;
    register int base asm("$16");
    int panel_y;
    register int dx asm("$5");
    register int dy asm("$6");
    int sum;
    int displayed;
    register int intermediate asm("$2");
    MenuWidgetNode *arg_panel;
    int panel_offset;

    if (!MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 7))) {
        selection = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (selection == 0) selection = MenuWidget_FindByModeAndSelectedBase(1, 0xB);

        if (MenuWidget_FindByModeAndSelectedBase(1, 0xB) == 0 &&
            MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 7)) >= 0) {
            index = Inv_GetPackedListItem(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 7)));
        } else {
            index = Inv_RestoreSelection(0);
        }

        item = Inv_LookupActiveListData(index);
        arg_panel = panel;
        dx = selection->x - arg_panel->x;
        dy = ((item->tailCount + 1) >> 1) * 16;
        panel_offset = arg_panel->y - 0x6C;
        dy -= panel_offset;
    } else {
        base = D_8009CF0C ? 0x38 : 0x24;
        panel_y = panel->y;
        if (Inv_GetPackedListCount() < 9) {
            intermediate = base + Inv_GetPackedListCount() * 16;
            dy = intermediate - panel_y;
        } else {
            intermediate = panel_y - 0x80;
            dy = base - intermediate;
        }
        arg_panel = panel;
        dx = 0;
    }

    MenuWidget_OffsetPosition(arg_panel, dx, dy);
    Draw_OffsetCursor(4, 4);
    Draw_PrintTextById(0x6A);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintTextById(0x6B);

    sum = D_800A1888[0] + D_800A1890[0];
    if (sum < 100 && !D_8009CF0C) {
        Draw_OffsetCursor(-0xF, 3);
        Draw_PrintNumberWidth2Unk(sum);
    } else {
        Draw_OffsetCursor(-0x14, 3);
        displayed = 999;
        if (sum < 1000) displayed = sum;
        Draw_PrintNumberWidth3Unk(displayed);
    }

    sum = D_800A188C[0] + D_800A1894[0];
    if (sum < 100 && !D_8009CF0C) {
        Draw_OffsetCursor(0x32, 0);
        Draw_PrintNumberWidth2Unk(sum);
    } else {
        Draw_OffsetCursor(0x2D, 0);
        displayed = 999;
        if (sum < 1000) displayed = sum;
        Draw_PrintNumberWidth3Unk(displayed);
    }
}
