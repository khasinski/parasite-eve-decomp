#include "pe1/battle_status.h"

typedef union BattleStatusPrimAddress {
    BattleStatusLinePrim *line;
    unsigned int *words;
    u32 address;
} BattleStatusPrimAddress;

void Battle_LayoutStatusPrimRow(int bottomY)
{
    BattleStatusLinePrim *line;
    BattleStatusPrimAddress bufferBase;
    unsigned int **orderingBase;
    int savedBottomY;
    int slot;
    u8 row;

    savedBottomY = bottomY;
    row = 0;
    bufferBase.line = D_8009E358;
    orderingBase = D_800B0E38;
    do {
        slot = g_ActiveDrawSlot;
        line = (BattleStatusLinePrim *)
            (((slot * 3) << 4) + (row << 4) + bufferBase.address);
        line->x0 = D_8009E358[slot * 3].x0 + row;
        line->y0 = D_8009E358[slot * 3].y0 + row;
        line->x1 = 0x50 - (row * 2);
        line->y1 = (s8)savedBottomY - (row * 2);
        AddPrim(orderingBase[slot] + (7 - row),
                (unsigned int *)(((slot * 3) << 4) +
                    ((row << 4) + bufferBase.address)));
        row++;
    } while ((u8)row < 3);
}
