/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct MenuData {
    char pad0[7];
    unsigned char field_7;
    unsigned char field_8;
    unsigned char field_9;
    char padA[4];
    short field_E;
    short field_10;
    short field_12;
} MenuData;

extern int Inv_RestoreSelection(int arg0);
extern MenuData *Inv_LookupActiveListData(int arg0);
extern void Draw_OffsetCursor(int arg0, int arg1);
extern void Sfx_DrawActiveListSlot(int arg0);
extern void Menu_DrawEquipStatsDelta(MenuData *data);
extern void Draw_PrintNumberWidth4Unk(int arg0);
extern void Draw_PrintSignedNumberWidth4(int arg0);
extern void Draw_AllocSprite(int arg0);

void Menu_DrawEquipStats(void) {
    int value;
    MenuData *data;

    value = Inv_RestoreSelection(1);
    data = Inv_LookupActiveListData(value);

    Draw_OffsetCursor(4, 4);
    Sfx_DrawActiveListSlot(value);
    Draw_OffsetCursor(-4, -4);
    Menu_DrawEquipStatsDelta(data);

    if (data != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(data->field_9);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->field_12);

        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(data->field_8);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->field_10);

        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(data->field_7);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->field_E);

        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}
