#include "pe1/menu_widget.h"

extern MenuWidgetNode *MenuWidget_GetCurrentNode(void);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern int Save_GetMetadataWindowIndex(void);
extern void Save_LoadMetadataWindowText(void);
extern int Save_DeleteMetadataChar(void);
extern unsigned char *Save_GetMetadataCursor(void);
extern char *Save_GetActiveMetadataBuffer(void);
extern void Inv_SetActiveList(int list_id, int arg1);
extern void Menu_CopyPromptCodes(unsigned char *src);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);
extern void Menu_PlayMoveSound(void);
extern void Menu_PlayErrorSound(void);

extern int D_8009D004;
extern unsigned int D_800B0CD8;

#define MENU_INPUT_BACK 0x800
#define MENU_INPUT_UP 0x1000
#define MENU_INPUT_RIGHT 0x2000
#define MENU_INPUT_DOWN 0x4000
#define MENU_INPUT_CONFIRM 0x10000

#define MODE_NAME_WINDOW_0 0x17
#define MODE_NAME_WINDOW_1 0x18
#define MODE_NAME_WINDOW_2 0x19
#define MODE_NAME_GRID 0x11

#define VALIDATE_METADATA_CURSOR(cursor_, valid_)       \
    do {                                                \
        unsigned char *scan_;                           \
        int only_separators_;                           \
                                                        \
        scan_ = (cursor_);                              \
        only_separators_ = 1;                           \
        if (*scan_ != 0xFF) {                           \
            do {                                        \
                if (*scan_ != 0xF) {                    \
                    only_separators_ = 0;               \
                }                                       \
                scan_++;                                \
            } while (*scan_ != 0xFF);                   \
        }                                               \
        (valid_) = ((cursor_) < scan_) && !only_separators_; \
    } while (0)

int Menu_StepNameEntryAlt(void *arg0, unsigned int input) {
    MenuWidgetNode *node;
    MenuWidgetNode *target;
    unsigned char *cursor;
    int window_index;
    int selected_offset;
    int next_cursor_y;
    int valid;
    int mode;

    (void)arg0;
    node = MenuWidget_GetCurrentNode();

    if (input & MENU_INPUT_CONFIRM) {
        mode = node->selected_base;
        if (mode == MODE_NAME_WINDOW_0) {
            input |= MENU_INPUT_RIGHT;
        } else if (mode == MODE_NAME_WINDOW_1) {
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
        } else if (mode == MODE_NAME_WINDOW_2) {
            cursor = Save_GetMetadataCursor();
            VALIDATE_METADATA_CURSOR(cursor, valid);
            if (valid) {
                Inv_SetActiveList(9, 0);
                if (D_8009D004 == 0) {
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                }
                D_800B0CD8 &= 0xFFFF7FFF;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
        }
    }

    if (input & MENU_INPUT_RIGHT) {
        window_index = Save_GetMetadataWindowIndex();
        selected_offset = (window_index << 1) + window_index;
        mode = node->selected_base;

        if (mode == MODE_NAME_WINDOW_0) {
            next_cursor_y = node->cursor_y - selected_offset;
        } else if (mode == MODE_NAME_WINDOW_1) {
            next_cursor_y = node->cursor_y - (selected_offset - 3);
        } else {
            next_cursor_y = node->cursor_y - (selected_offset - 5);
        }

        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        target = MenuWidget_FindByModeAndSelectedBase(2, MODE_NAME_GRID);
        target->cursor_x = 0;
        if ((next_cursor_y < 5 - selected_offset) || (target->y_limit < 6 - selected_offset)) {
            target->cursor_y = next_cursor_y;
        }
        MenuWidget_SetCurrentNode(target);
        Menu_PlayMoveSound();
        return 1;
    }

    if (input & MENU_INPUT_UP) {
        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        window_index = Save_GetMetadataWindowIndex();
        mode = node->selected_base;
        if (window_index + MODE_NAME_WINDOW_0 < mode) {
            mode--;
        } else {
            mode = MODE_NAME_WINDOW_2;
        }

        target = MenuWidget_FindByModeAndSelectedBase(2, mode);
        target->cursor_x = 0;
        target->cursor_y = target->y_limit - 1;
        MenuWidget_SetCurrentNode(target);
        Menu_PlayMoveSound();
        return 1;
    }

    if (input & MENU_INPUT_DOWN) {
        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        mode = node->selected_base;
        if (mode >= MODE_NAME_WINDOW_2) {
            mode = Save_GetMetadataWindowIndex() + MODE_NAME_WINDOW_0;
        } else {
            mode++;
        }

        target = MenuWidget_FindByModeAndSelectedBase(2, mode);
        target->cursor_x = 0;
        target->cursor_y = 0;
        MenuWidget_SetCurrentNode(target);
        Menu_PlayMoveSound();
        return 1;
    }

    if (input & MENU_INPUT_BACK) {
        if (node->selected_base != MODE_NAME_WINDOW_2) {
            node->cursor_x = -1;
            target = MenuWidget_FindByModeAndSelectedBase(2, MODE_NAME_WINDOW_2);
            target->cursor_x = 0;
            MenuWidget_SetCurrentNode(target);
            return 1;
        }

        cursor = Save_GetMetadataCursor();
        VALIDATE_METADATA_CURSOR(cursor, valid);
        if (valid) {
            Inv_SetActiveList(9, 0);
            if (D_8009D004 == 0) {
                Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
            }
            D_800B0CD8 &= 0xFFFF7FFF;
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }

    return 1;
}
