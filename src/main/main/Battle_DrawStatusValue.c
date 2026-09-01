#include "pe1/battle_status.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct ActiveDrawSlotGlobal {
    int value;
    u8 pad04[12];
} ActiveDrawSlotGlobal;

extern ActiveDrawSlotGlobal g_ActiveDrawSlotObject0
    __asm__("D_8009CDDC");
extern ActiveDrawSlotGlobal g_ActiveDrawSlotObject1
    __asm__("D_8009CDDC");
extern u16 D_8009E360[];
extern u16 D_8009E362[];

void Battle_DrawStatusValue(int value, int yOffset)
{
    int drawSlot;
    int anchorOffset;
    int markerOffset;
    u8 *markerBase;
    int activeSlot;
    BattleStatusMarkerBody *marker;
    int savedValue;
    register u64 x;
    u16 y;
    u32 drawSlotOffset;

    activeSlot = g_ActiveDrawSlotObject0.value;
    savedValue = value;
    drawSlot = activeSlot;
    markerBase = (u8 *)D_8009E888;
    anchorOffset = (drawSlot * 3) << 4;
    markerOffset = drawSlot * sizeof(BattleStatusMarkerBody);
    do {
        do {
            marker = (BattleStatusMarkerBody *)(markerBase + markerOffset);
        } while (0);
        drawSlotOffset = drawSlot << 2;
        markerBase -= 8;
        y = *(u16 *)((u8 *)D_8009E362 + anchorOffset);
        x = (marker->x =
            *(u16 *)((u8 *)D_8009E360 + anchorOffset) + 8);
        y += yOffset;
    } while (0);
    marker->y = y;

    AddPrim(*(unsigned int **)((u8 *)D_800B0E38 + drawSlotOffset) + 4,
            (unsigned int *)(markerBase + markerOffset));
    drawSlot = g_ActiveDrawSlotObject1.value;
    Battle_DrawDecimalNumber(
        D_8009E8B8[drawSlot],
        *(u16 *)((u8 *)D_8009E360 + drawSlot * 3 * 0x10) + 0x40,
        (s16)--y, (s16)savedValue, 1);
}
