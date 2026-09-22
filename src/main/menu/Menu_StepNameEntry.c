#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"
#include "pe1/text.h"
#include "pe1/save.h"
#include "pe1/game_state.h"
/* CC1_FLAGS: -G4096 */
/* MASPSX_FLAGS: -G8 */
/* GCC keeps the large game-state reference symbolic; MASPSX still applies
 * the retail -G8 threshold to actual GP-relative accesses. */

int Menu_StepNameEntry(MenuWidgetNode *parent, unsigned int flags)
{
    int row, index, blank;
    MenuWidgetNode *page, *node;
    node = MenuWidget_GetChild(parent, 0);
    if (flags & 0x8000) {
        index = Save_GetMetadataWindowIndex();
        node->cursor_x = -1;
        row = node->cursor_y + index * 3;
        if (row >= 0) {
            if (row < 3) {
                node = MenuWidget_FindByModeAndSelectedBase(2, 23);
                node->cursor_y = row;
            } else if (row < 5) {
                node = MenuWidget_FindByModeAndSelectedBase(2, 24);
                node->cursor_y = row - 3;
            } else {
                node = MenuWidget_FindByModeAndSelectedBase(2, 25);
                node->cursor_y = 0;
            }
        } else {
            node = MenuWidget_FindByModeAndSelectedBase(2, 25);
            node->cursor_y = 0;
        }
        node->cursor_x = 0;
        MenuWidget_SetCurrentNode(node);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        unsigned char *text;
        page = MenuWidget_FindByModeAndSelectedBase(2, 23);
        index = 0x75;
        if (page)
            index = page->cursor_y + 0x73;
        text = Str_LookupTable4(index);
        D_8009CF54 = text;
        Save_AppendMetadataChar(text[MenuWidget_GridCellIndex(node)]);
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        if (Save_DeleteMetadataChar())
            Menu_PlayCancelSound();
        else
            Menu_PlayErrorSound();
    } else if (flags & 0x800) {
        if (node->selected_base != 25) {
            node->cursor_x = -1;
            page = MenuWidget_FindByModeAndSelectedBase(2, 25);
            page->cursor_x = 0;
            MenuWidget_SetCurrentNode(page);
        } else {
            unsigned char *text = Save_GetMetadataCursor();
            unsigned char *end;
            blank = 1;
            for (end = text; *end != 0xFF;)
                blank &= *end++ == 0x0F;
            if (end > text && !blank) {
                Inv_SetActiveList(9, 0);
                if (!D_8009D004)
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                g_GameState.flags &= ~0x8000;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
        }
    }
    return 1;
}
