#ifndef PE1_INVENTORY_SLOTS_H
#define PE1_INVENTORY_SLOTS_H

#include "common.h"
#include "pe1/inventory.h"

extern s16 D_800C0E48[];
/* Retail adjacency: tracked selections at C0E20, 50 item IDs at C0E48,
 * and 128 equipment records at C0EAC. Reserved bytes are not interpreted. */
typedef struct InventoryRuntime {
    s8 tracked[4];
    u8 reserved04[0x24];
    s16 slots[50];
    ItemDataRecord equipment[128];
} InventoryRuntime;

PE1_STATIC_ASSERT(PE1_OFFSETOF(InventoryRuntime, slots) == 0x28,
                  inventory_runtime_slots_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InventoryRuntime, equipment) == 0x8C,
                  inventory_runtime_equipment_offset);
extern InventoryRuntime D_800C0E20;
extern s32 D_8009D040;
extern s16 D_800A1D9C[];
extern u8 D_8009DE64[];
void Inv_SetActiveList(int mode, int *slot);
extern u8 D_800C0E0C;
/* Separate views preserve retail's address reload across bonus queries.
 * Matching debt: these aliases name the same byte, not distinct storage. */
extern u8 g_InvBaseCapacityForLimit[] asm("D_800C0E0C");
extern u8 g_InvBaseCapacityForResult[] asm("D_800C0E0C");

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
#define g_InvTrackedSlots D_800C0E20.tracked
#define g_InvBaseCapacity D_800C0E0C

int Inv_GetAyaSlotLimit(void);
int Inv_GetBonusSlotCount(void);
int Inv_FindFirstEmptySlot(int neededSlots);
void Inv_CompactActiveListSlots(void);
int Inv_CheckFreeSlotCapacity(int requestedSlots);

#endif
