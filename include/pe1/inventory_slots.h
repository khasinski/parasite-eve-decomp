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
extern s32 D_8009D044;
extern s16 D_800A1E00[];
extern s16 *D_8009D07C;
extern s32 D_8009D080;
extern s16 D_800C1F80[];
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
extern s32 D_8009D068;
/* Equipment comparison filter: nonzero selects kinds 1..8, zero kind 9; first-stat floor. */
extern s32 D_8009D0CC, D_8009D0D0;
/* Pending item IDs and the current number to transfer. */
extern s32 D_8009D078;
extern s16 D_800A1FD4[];

#define g_AyaInventoryItems D_800C0E48
#define g_InvItemPtr D_8009D048
#define g_InvSlotLimit D_8009D050
#define g_InvSelectionBitStorage D_8009D05C
#define g_InvSelectionBits D_8009D058
#define g_InvSelectionBitWords D_8009D064
#define g_InvPendingCompactCount D_8009D06C
#define g_InvTrackedSlots D_800C0E20.tracked
#define g_InvBaseCapacity D_800C0E0C

/* Two remembered selections. The retail indexed accesses use full addresses;
 * fixed-slot accesses use the individual small-data symbols below. Keeping
 * incomplete arrays avoids the old, overlapping three-element declarations. */
extern s32 g_InvSavedSelectionIndex[];
extern s32 g_InvSavedSelectionFromStorage[];
extern s32 D_8009D090, D_8009D094; /* indices 0, 1 */
extern s32 D_8009D098, D_8009D09C; /* storage flags 0, 1 */
extern s16 *g_InvActiveListOverride;
extern s32 g_InvOverrideSlotLimit;
extern u32 g_InvStorageSelectionBits[];
#define g_AyaItemSelectionBits D_8009D05C

/* Scratch state and full record snapshots used by the item-detail operation. */
extern u8 D_800A1FE8[];
extern u8 *D_8009D084;
extern s32 D_8009D088, D_8009D08C;
extern ItemDataRecord D_800A204C, D_800A206C;
/* Global fallback and category-specific ammunition records; storage tool IDs. */
extern ItemDataRecord D_800A1E44;
extern ItemDataRecord D_800A1E64[];
extern s16 D_800C1EB8[];

void Inv_InitWayneStorage(void);
void Inv_RememberSelection(unsigned int index, int value);
int Inv_RestoreSelection(unsigned int index);
void Inv_BuildStorageDisplay(void);

int Inv_GetAyaSlotLimit(void);
int Inv_GetBonusSlotCount(void);
int Inv_FindFirstEmptySlot(int neededSlots);
void Inv_CompactActiveListSlots(void);
int Inv_CheckFreeSlotCapacity(int requestedSlots);

#endif
