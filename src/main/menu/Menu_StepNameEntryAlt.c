/* CC1_FLAGS: -G4096 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"
#include "pe1/save.h"
#include "pe1/game_state.h"

#define MENU_INPUT_BACK 0x800
#define MENU_INPUT_UP 0x1000
#define MENU_INPUT_RIGHT 0x2000
#define MENU_INPUT_DOWN 0x4000
#define MENU_INPUT_CONFIRM 0x10000

#define MODE_NAME_WINDOW_0 0x17
#define MODE_NAME_WINDOW_1 0x18
#define MODE_NAME_WINDOW_2 0x19
#define MODE_NAME_GRID 0x11

int Menu_StepNameEntryAlt(MenuWidgetNode *parent, unsigned int input) {
    MenuWidgetNode *node;
    MenuWidgetNode *target;
    MenuWidgetNode *move_node;
    int window_index;
    int selected_offset;
    int next_cursor_y;

    (void)parent;
    node = MenuWidget_GetCurrentNode();

    if (input & MENU_INPUT_CONFIRM) {
        int mode = node->selected_base;
        switch (mode) {
        case MODE_NAME_WINDOW_0:
            input |= MENU_INPUT_RIGHT;
            break;
        case MODE_NAME_WINDOW_1:
            if (node->cursor_y != 0) {
                Save_LoadMetadataWindowText();
                Menu_PlayConfirmSound();
            } else {
                if (Save_DeleteMetadataChar() != 0) {
                    Menu_PlayCancelSound();
                } else {
                    Menu_PlayErrorSound();
                }
            }
            break;
        case MODE_NAME_WINDOW_2: {
            unsigned char *text = Save_GetMetadataCursor();
            unsigned char *end;
            int blank = 1;

            for (end = text; *end != 0xFF;) {
                blank &= (*end++ == 0x0F);
            }

            if (end > text && !blank) {
                Inv_SetActiveList(9, 0);
                if (D_8009D004 == 0) {
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                }
                g_GameState.flags &= 0xFFFF7FFF;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
    }
    }

    if (input & MENU_INPUT_RIGHT) {
        int mode;
        int cursor_y;
        window_index = Save_GetMetadataWindowIndex();
        selected_offset = (window_index << 1) + window_index;
        mode = node->selected_base;
        cursor_y = node->cursor_y;

        if (mode != MODE_NAME_WINDOW_0) {
            int offset;
            if (mode == MODE_NAME_WINDOW_1) {
                offset = selected_offset - 3;
                next_cursor_y = cursor_y - offset;
            } else {
                offset = selected_offset - 5;
                next_cursor_y = cursor_y - offset;
            }
        } else {
            next_cursor_y = cursor_y - selected_offset;
        }

        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, MODE_NAME_GRID);
        node->cursor_x = 0;
        if ((next_cursor_y < 5 - selected_offset) || (node->cursor_y < 6 - selected_offset)) {
            node->cursor_y = next_cursor_y;
        }
        move_node = node;
        goto moved;
    }

    if (input & MENU_INPUT_UP) {
        int mode;
        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        window_index = Save_GetMetadataWindowIndex();
        {
            int current = node->selected_base;
            mode = MODE_NAME_WINDOW_2;
            if (window_index + MODE_NAME_WINDOW_0 < current) {
                mode = current - 1;
            }
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, mode);
        node->cursor_x = 0;
        node->cursor_y = node->y_limit - 1;
        move_node = node;
        goto moved;
    }

    if (input & MENU_INPUT_DOWN) {
        int mode;
        int current;
        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        current = node->selected_base;
        if (current < MODE_NAME_WINDOW_2) {
            mode = current + 1;
        } else {
            mode = Save_GetMetadataWindowIndex() + MODE_NAME_WINDOW_0;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, mode);
        node->cursor_x = 0;
        node->cursor_y = 0;
        move_node = node;
        goto moved;
    }

    goto after_move;

moved:
    MenuWidget_SetCurrentNode(move_node);
    Menu_PlayMoveSound();
    return 1;

after_move:
    if (input & MENU_INPUT_BACK) {
        if (node->selected_base != MODE_NAME_WINDOW_2) {
            node->cursor_x = -1;
            target = MenuWidget_FindByModeAndSelectedBase(2, MODE_NAME_WINDOW_2);
            target->cursor_x = 0;
            MenuWidget_SetCurrentNode(target);
            return 1;
        }

        {
            unsigned char *text = Save_GetMetadataCursor();
            unsigned char *end;
            int blank = 1;

            for (end = text; *end != 0xFF;) {
                blank &= (*end++ == 0x0F);
            }

            if (end > text && !blank) {
                Inv_SetActiveList(9, 0);
                if (D_8009D004 == 0) {
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                }
                g_GameState.flags &= 0xFFFF7FFF;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
        }
    }

    return 1;
}
