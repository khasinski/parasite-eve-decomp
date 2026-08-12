#include "pe1/battle_status.h"

void Battle_DrawStatusValue(int value, int yOffset) {
    BattleStatusLinePrim *lineBuffer;
    BattleStatusMarkerBody *marker;
    int markerY;

    lineBuffer = &D_8009E358[g_ActiveDrawSlot * 3];
    marker = &D_8009E888[g_ActiveDrawSlot];
    marker->x = lineBuffer->x0 + 8;
    markerY = lineBuffer->y0 + yOffset;
    marker->y = markerY;
    AddPrim(D_800B0E38[g_ActiveDrawSlot] + 4,
            (unsigned int *)((unsigned char *)marker - 8));

    Battle_DrawDecimalNumber(D_8009E8B8[g_ActiveDrawSlot],
                             lineBuffer->x0 + 0x40,
                             (short)(markerY - 1),
                             (short)value,
                             1);
}
