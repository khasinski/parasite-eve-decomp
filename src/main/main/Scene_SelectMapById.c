typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 D_8009CD78[];
extern u8 D_8009CD80[];
extern u8 g_MapIdTable[];
extern u8 g_MapSelectIndexTable[];
extern u8 g_MapFilenameTable[];
extern int D_8009D280;
extern int D_8009D1C4;
extern u32 D_8009D1A0;
extern u32 D_800B0CD8[];

int Menu_IsEquipSlotActive(int **args);
int Render_GetOrLoadFontGlyph(int glyph);
int Render_LoadFontGlyphAlt(void);
int Menu_GetEquipSlotStateOrIndex(void);
void Menu_ResetEquipSlotState(void);
int Task_GpuPackPrimColor(int start, int end);
int Str_ParseBase32Id(u8 *text);

int Scene_SelectMapById(int **args) {
    int raw_selection;
    u8 selection;
    int index;
    u8 state;
    u8 *select_index;
    u32 map_flags;
    u32 state_flags;

    if ((u8)Menu_IsEquipSlotActive(args) != 0) {
        raw_selection = Render_GetOrLoadFontGlyph(*(u8 *)args[0]);
    } else {
        raw_selection = Render_LoadFontGlyphAlt();
    }
    selection = raw_selection;

    if (selection == 0xFF) {
        state = Menu_GetEquipSlotStateOrIndex();
        if (state < 2) {
            Menu_ResetEquipSlotState();
            D_8009D280 = Str_ParseBase32Id(D_8009CD78);
        } else {
            Menu_ResetEquipSlotState();
            D_8009D280 = Str_ParseBase32Id(D_8009CD80);
        }
        return 1;
    }

    if ((u8)(raw_selection - 6) < 2) {
        if ((s16)Task_GpuPackPrimColor(0, 100) < 60) {
            D_8009D280 = Str_ParseBase32Id(g_MapIdTable);
        } else {
            D_8009D280 = Str_ParseBase32Id(g_MapIdTable + ((s16)Task_GpuPackPrimColor(0, 4) << 3));
        }
    } else {
        state = (u8)(Menu_GetEquipSlotStateOrIndex() - 1);
        index = state / 10;
        select_index = g_MapSelectIndexTable + index * 24;
        D_8009D280 = Str_ParseBase32Id(g_MapFilenameTable + ((int)select_index[selection] << 3));
    }

    if (D_8009D1C4 == D_8009D280) {
        map_flags = D_800B0CD8[0];
        state_flags = D_8009D1A0;
        state_flags |= 0x2000;
        map_flags |= 0x800;
        D_8009D1A0 = state_flags;
        D_800B0CD8[0] = map_flags;
    }
    return 1;
}
