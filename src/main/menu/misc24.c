/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;

extern struct { char _[16]; } D_8009234C_o __asm__("D_8009234C");
extern struct { char _[16]; } D_800C0E20_o __asm__("D_800C0E20");
extern struct { char _[16]; } D_800C0E22_o __asm__("D_800C0E22");

#define D_8009234C ((u8 *)&D_8009234C_o)
#define D_800C0E20 (*(s8 *)&D_800C0E20_o)
#define D_800C0E22 (*(s8 *)&D_800C0E22_o)
#define W32(ptr, offset) (*(int *)((u8 *)(ptr) + (offset)))
#define U8(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))

extern int D_8009CEF0;
extern int D_8009CEFC;
extern int D_8009CF0C;
extern int D_8009CF18;
extern int D_8009CF1C;
extern u8 *D_8009CF20;
extern int D_8009CF50;
extern int D_8009CF8C;
extern int D_8009CFF8;

void *MenuWidget_GetCurrentNode(void);
void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void MenuWidget_DestroyNode(void *arg0);
int MenuWidget_GridCellIndex(void *arg0);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_OffsetCursor(int x, int y);
int GameTime_GetCounterSeconds(int mode);
void Draw_PrintTimeValue(int value, int variant);
void Draw_AllocSprite(int id);
void Draw_SetColor(int color);
void Draw_SetTextDimmed(int mode);
void Draw_PrintTextById(int id);
void Draw_PrintRawText(void *text);
int Menu_GetBattleEquipMode(void);
void *Inv_LookupActiveListData(int index);
int Inv_GetWayneListItemByIndex(int index);
int Inv_GetActiveListItemType(int index);
void *Inv_LookupItemData(int index);
int Inv_RestoreSelection(int mode);
int Inv_GetPackedListItem(int index);
int Inv_GetPackedListCount(void);
void Inv_SelectActiveList(int list);
int Menu_GetBattleCountEntry(int index);
void *Str_LookupTable4(int id);
void *Str_LookupTable10(int id);
void *Str_LookupTableC(int id);
int MemCard_PollTransferDelay(void);
int MemCard_IsPortPresent(int port);
int Save_GetTitleStyleFlag(void);

void Menu_CloseContextHelpPanel(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
}

void Menu_DrawContextHelpText(void *node) {
    void *current;
    void *grid;
    void *entry;
    void *text;
    int mode;
    int selected_base;
    int grid_index;
    int id;
    int bits;
    int item;
    int item_type;

    text = 0;
    current = MenuWidget_GetCurrentNode();

    Draw_StatePush();
    Draw_OffsetCursor(W32(node, 0x34) - 0x27, 1);
    Draw_PrintTimeValue(GameTime_GetCounterSeconds(2), 1);
    Draw_OffsetCursor(-0x21, 0);
    Draw_AllocSprite(0x8C);
    Draw_StatePop();

    if (current != 0) {
        selected_base = W32(current, 0x24);
        grid = MenuWidget_FindByModeAndSelectedBase(2, selected_base);
        grid_index = MenuWidget_GridCellIndex(grid);

        if ((unsigned int)selected_base < 0x3D) {
            switch (selected_base) {
            case 0:
                bits = D_8009CEF0;
                if (Menu_GetBattleEquipMode() != 0) {
                    bits &= 0x1F;
                } else {
                    bits &= 0x1EF;
                }

                id = -1;
                while (grid_index >= 0 && id < 9) {
                    grid_index -= bits & 1;
                    id++;
                    bits >>= 1;
                }
                text = Str_LookupTable10(id + 0x28);
                break;

            case 1:
                if (D_8009CF8C < 0) {
                    entry = Inv_LookupActiveListData(grid_index);
                    if (entry != 0) {
                        text = Str_LookupTableC(U8(entry, 4) - 1);
                    }
                    break;
                }
                goto active_type_help;

            case 5:
                if (grid_index >= 0) {
                    if (D_8009CF18 != 0) {
                        entry = Inv_LookupActiveListData(D_800C0E20);
                    } else {
                        entry = Inv_LookupActiveListData(D_800C0E22);
                    }
                    if (entry != 0) {
                        text = Str_LookupTableC(U8(entry, 4) - 1);
                    }
                }
                break;

            case 6:
                id = U8(D_8009CF20 + grid_index, 0x15) & 0x1F;
                if (id != 0) {
                    text = Str_LookupTable10(id + ((1 - D_8009CF18) * 20) - 1);
                }
                break;

            case 7:
                if (D_8009CF1C != 0) {
                    item = Inv_RestoreSelection(1);
                } else {
                    item = Inv_GetPackedListItem(grid_index);
                }
                entry = Inv_LookupActiveListData(item);
                if (entry != 0) {
                    text = Str_LookupTableC(U8(entry, 4) - 1);
                }
                break;

            case 8:
                if (grid_index < Inv_GetPackedListCount()) {
                    text = Str_LookupTableC(Inv_GetPackedListItem(grid_index) + 0xEB);
                }
                break;

            case 11:
                entry = Inv_LookupActiveListData(Inv_RestoreSelection(1));
                id = U8((u8 *)entry + grid_index, 0x15) & 0x1F;
                if (id != 0) {
                    text = Str_LookupTable10(id + ((1 - D_8009CF18) * 20) - 1);
                }
                break;

            case 12:
                text = Str_LookupTable10(grid_index + 0x4F);
                break;

            case 13:
            case 16:
                if (D_8009CEFC != 0) {
                    text = Str_LookupTableC(Menu_GetBattleCountEntry(grid_index) - 1);
                } else {
                    entry = Inv_LookupActiveListData(Inv_GetPackedListItem(grid_index));
                    if (entry != 0) {
                        text = Str_LookupTableC(U8(entry, 4) - 1);
                    }
                }
                break;

            case 14:
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(grid_index);
                if (entry != 0) {
                    text = Str_LookupTableC(U8(entry, 4) - 1);
                }
                break;

            case 17:
                text = Str_LookupTable4(0x27);
                break;

            case 27:
            case 28:
                text = Str_LookupTable10(0x52);
                break;

            case 29:
                text = Str_LookupTable10(grid_index - ((D_8009CF18 * 3) - 0x56));
                break;

            case 32:
                if (grid_index < 4) {
                    text = Str_LookupTable10(grid_index + 0x31);
                }
                break;

            case 33:
                text = Str_LookupTable10(grid_index + 0x3A);
                break;

            case 35:
                text = Str_LookupTable10(grid_index + 0x3D);
                break;

            case 36:
                if (MemCard_PollTransferDelay() != 0) {
                    Draw_OffsetCursor(6, 5);
                    Draw_PrintTextById(0x4C);
                    Draw_OffsetCursor(0, 0x10);
                    Draw_PrintTextById(0x4D);
                } else {
                    id = 0;
                    if (MemCard_IsPortPresent(0) != 0 || MemCard_IsPortPresent(1) != 0) {
                        id = 1;
                    }
                    text = Str_LookupTable4(id | 0x4E);
                }
                break;

            case 37:
            case 38:
                if (D_8009CF50 != 0) {
                    if (D_8009CFF8 != 0) {
                        Draw_SetTextDimmed(0);
                        Draw_OffsetCursor(6, 5);
                        Draw_PrintTextById(0x58);
                        Draw_OffsetCursor(0, 0x10);
                        Draw_PrintTextById(0x59);
                    } else {
                        text = Str_LookupTable4(0x5C);
                    }
                } else {
                    if (Save_GetTitleStyleFlag() != 0) {
                        text = Str_LookupTable4(0x57);
                    } else {
                        text = Str_LookupTable4(0x5B);
                    }
                }
                break;

            case 39:
                Draw_SetTextDimmed(0);
                Draw_OffsetCursor(6, 5);
                Draw_PrintTextById(0x50);
                Draw_OffsetCursor(0, 0x10);
                Draw_PrintTextById(0x51);
                break;

            case 46:
                text = Str_LookupTable10(grid_index + 0x3F);
                break;

            case 48:
                text = Str_LookupTable10(grid_index + 0x38);
                break;

            case 49:
                text = Str_LookupTable10(grid_index + 0x42);
                break;

            case 50:
                if (D_8009CF0C == 1 && grid_index == 2) {
                    id = D_8009234C[4];
                } else {
                    id = D_8009234C[grid_index];
                }
                text = Str_LookupTable10(id);
                break;

            case 51:
                if (D_8009CF8C < 0) {
                    item = Inv_GetWayneListItemByIndex(grid_index);
                    entry = Inv_LookupActiveListData(item);
                    if (entry != 0) {
                        text = Str_LookupTableC(U8(entry, 4) - 1);
                    }
                    break;
                }
                goto active_type_help;

            case 52:
                if (D_8009CF8C < 0) {
                    entry = Inv_LookupItemData(grid_index);
                    if (entry != 0) {
                        text = Str_LookupTableC(U8(entry, 4) - 1);
                    }
                    break;
                }

active_type_help:
                item_type = Inv_GetActiveListItemType(D_8009CF8C);
                mode = 0;
                if (item_type < 0x13 || item_type >= 0x16) {
                    mode = 1;
                }
                text = Str_LookupTable4(mode | 0xE);
                Draw_SetColor(0x408040);
                break;

            case 56:
                Draw_SetTextDimmed(0);
                text = Str_LookupTable10(0x44);
                break;

            case 58:
                text = Str_LookupTable10(grid_index + 0x35);
                break;

            case 59:
                if (D_8009CF0C == 2) {
                    text = Str_LookupTable10(grid_index + 0x4D);
                } else {
                    text = Str_LookupTable10(grid_index + 0x4B);
                }
                break;

            case 60:
                text = Str_LookupTable10(grid_index - ((D_8009CF18 * 3) - 0x48));
                break;
            }
        }
    }

    if (text != 0) {
        Draw_OffsetCursor(6, 5);
        Draw_PrintRawText(text);
        Draw_SetColor(0x808080);
    }
}
