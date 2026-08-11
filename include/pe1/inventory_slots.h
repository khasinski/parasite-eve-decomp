#ifndef PE1_INVENTORY_SLOTS_H
#define PE1_INVENTORY_SLOTS_H

#include "common.h"

extern s16 D_800C0E48[];
extern s8 D_800C0E20[];
extern u8 D_800C0E0C;
extern u32 D_8009D05C[];
extern s16 *D_8009D048;
extern s32 D_8009D050;
extern u32 *D_8009D058;
extern s32 D_8009D064;
extern s32 D_8009D06C;

#define g_AyaInventoryItems D_800C0E48
#define g_InvItemPtr D_8009D048
#define g_InvSlotLimit D_8009D050
#define g_InvSelectionBitStorage D_8009D05C
#define g_InvSelectionBits D_8009D058
#define g_InvSelectionBitWords D_8009D064
#define g_InvPendingCompactCount D_8009D06C
#define g_InvTrackedSlots D_800C0E20
#define g_InvBaseCapacity D_800C0E0C

int Inv_GetAyaSlotLimit(void);
int Inv_GetBonusSlotCount(void);
int Inv_FindFirstEmptySlot(int neededSlots);
void Inv_CompactActiveListSlots(void);
int Inv_CheckFreeSlotCapacity(int requestedSlots);

#endif
