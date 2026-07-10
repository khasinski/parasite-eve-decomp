typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;

#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define W(base, off) (*(int *)((u8 *)(base) + (off)))
#define CLAMP_EQUIP_PREVIEW(out, item, stat_off, active_stat)         \
    do {                                                             \
        (out) = S16((item), (stat_off));                              \
        if (D_8009CFC8 == (active_stat)) {                            \
            if ((out) + D_8009CFCC >= 1000) {                         \
                (out) = 999;                                          \
            } else {                                                  \
                (out) += D_8009CFCC;                                  \
            }                                                         \
        } else if ((out) >= 1000) {                                   \
            (out) = 999;                                              \
        }                                                            \
    } while (0)

extern int D_8009CF18;
extern int D_8009CF1C;
extern int D_8009CF30;
extern int D_8009CF34;
extern int D_8009CF38;
extern int D_8009CFC4;
extern int D_8009CFC8;
extern int D_8009CFCC;
extern s8 D_800C0E20;
extern s8 D_800C0E22;

void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
int MenuWidget_GridCellIndex(void *node);
int Inv_GetPackedListItem(int index);
void Inv_RememberSelection(int side, int item);
int Inv_RestoreSelection(int side);
void Draw_OffsetCursor(int x, int y);
void Sfx_DrawActiveListSlot(int slot);
void *Inv_LookupActiveListData(int slot);
void Menu_DrawEquipStatsDelta(void *item);
void Draw_PrintNumberWidth4Unk(int value);
void Draw_PrintSignedNumberWidth4(int value);
void Draw_AllocSprite(int glyph);
void Draw_SetStatCompareColor(int current, int compare);
void Draw_SetColor(int color);
void MenuWidget_SetCursorY(void *node);
void MenuWidget_ClearCursorY(void *node);

void Menu_StepEquipSlotSelect2(void) {
    void *node;
    void *cursor_node;
    void *item;
    int slot;
    int value;

    node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    if (node != 0) {
        if (W(node, 0x44) >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (D_8009CF30 != 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        if (node != 0 && W(node, 0x44) >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (D_8009CF1C == 0) {
        if (D_8009CF18 != 0) {
            Inv_RememberSelection(0, D_800C0E20);
        } else {
            Inv_RememberSelection(0, D_800C0E22);
        }
    }

    slot = Inv_RestoreSelection(0);
    Draw_OffsetCursor(4, 4);
    Sfx_DrawActiveListSlot(slot);
    Draw_OffsetCursor(-4, -4);

    item = Inv_LookupActiveListData(slot);
    D_8009CF18 = (U8(item, 6) != 9);
    Menu_DrawEquipStatsDelta(item);

    if ((D_8009CF1C != 0 || D_8009CF30 != 0 || D_8009CF34 != 0 || D_8009CF38 != 0) &&
        item != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(U8(item, 9));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(S16(item, 0x12));
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(U8(item, 8));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(S16(item, 0x10));
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(U8(item, 7));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(S16(item, 0xE));
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }

    if (D_8009CFC4 >= 0) {
        Draw_OffsetCursor(0x28, -0x32);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);
        Draw_PrintSignedNumberWidth4(S16(item, 0xE));
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(S16(item, 0x10));
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(S16(item, 0x12));

        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0xA, -0xA);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);

        CLAMP_EQUIP_PREVIEW(value, item, 0xE, 0);
        Draw_SetStatCompareColor(S16(item, 0xE), value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 0x10, 1);
        Draw_SetStatCompareColor(S16(item, 0x10), value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 0x12, 2);
        Draw_SetStatCompareColor(S16(item, 0x12), value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_SetColor(0x808080);
    }

    cursor_node = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    if (U8(item, 0x14) == 0) {
        MenuWidget_SetCursorY(cursor_node);
    } else {
        MenuWidget_ClearCursorY(cursor_node);
    }
}
