/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/aya.h"
#include "pe1/save.h"
#include "pe1/text.h"
#include "pe1/psyq_bios.h"
#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"

static inline void CopyName(u8 *dest, u8 *source) {
    while ((*dest++ = *source++) != 255) {}
}

static inline void InitName(u8 *buffer, int length, ItemDataRecord *item) {
    u8 *p, *source, *dest;
    g_SaveMetadataWindowIndex = 1;
    g_SaveMetadataSourceRecord = item;
    g_SaveMetadataCursor = buffer;
    g_SaveMetadataWindowLength = length;
    for (p = g_SaveMetadataCursor;
         p < g_SaveMetadataCursor + g_SaveMetadataWindowLength; p++)
        *p = 255;
    dest = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord)
        source = Str_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
    else
        source = Str_LookupTable4(30);
    CopyName(dest, source);
}

void Save_InitMetadataState(void) {
    bzero(&g_SaveRuntimeState, sizeof(g_SaveRuntimeState));
    /* Retail initializes the same window twice, including both lookups. */
    InitName(g_SaveRuntimeState.metadata[1].text, 8, 0);
    InitName(g_SaveRuntimeState.metadata[1].text, 8, 0);
    Menu_CopyPromptCodes(Str_LookupTable4(30));
    Inv_InitNewGameInventory();
    D_800C0E20.specialStorage[0] = 515;
    D_800C0E00.blend_color = 0x404040;
    Draw_BlendColor(0x404040);
    D_800A76A4[0] = 0;
    D_800A76B0[0] = 0;
    D_800A76BC[0] = 0;
    D_800A76C8[0] = 0;
    Draw_SetBaseOffsetPosition(0, 8 - Draw_GetBaseY());
    MenuWidget_SetColumnLayoutMode(0);
    Menu_SetActionSubmenuSelection(1);
    D_800C20A4[0] = 255;
    D_800C20B4[0] = 255;
}
