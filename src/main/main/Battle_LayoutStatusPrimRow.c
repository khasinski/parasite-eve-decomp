#include "pe1/battle_status.h"

void Battle_LayoutStatusPrimRow(int bottomY) {
    BattleStatusLinePrim *buffer;
    BattleStatusLinePrim *line;
    unsigned int *orderingTable;
    int row;

    buffer = &D_8009E358[g_ActiveDrawSlot * 3];
    orderingTable = D_800B0E38[g_ActiveDrawSlot];
    for (row = 0; row < 3; row++) {
        line = &buffer[row];
        line->x0 = buffer->x0 + row;
        line->y0 = buffer->y0 + row;
        line->x1 = 0x50 - (row * 2);
        line->y1 = (signed char)bottomY - (row * 2);
        AddPrim(orderingTable + (7 - row), (unsigned int *)line);
    }
}
