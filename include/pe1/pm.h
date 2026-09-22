#ifndef PE1_PM_H
#define PE1_PM_H

#include "common.h"

/* Both process-manager banks share this prefix. Pm_Exec increments ticks
 * at +4 after calling the command handler; +8 identifies the owning object. */
typedef struct PmSlotHeader {
    u8 state;
    u8 command;
    u8 field02;
    u8 field03;
    u32 ticks;
    void *owner;
} PmSlotHeader;

typedef struct PmPrimarySlot {
    PmSlotHeader header;
    u8 reserved0C[0xA00];
} PmPrimarySlot;

typedef struct PmSecondarySlot {
    PmSlotHeader header;
    u8 reserved0C[0x100];
} PmSecondarySlot;

PE1_STATIC_ASSERT(sizeof(PmSlotHeader) == 0x0C, pm_slot_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmSlotHeader, ticks) == 4, pm_slot_ticks_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmSlotHeader, owner) == 8, pm_slot_owner_offset);
PE1_STATIC_ASSERT(sizeof(PmPrimarySlot) == 0xA0C, pm_primary_slot_size);
PE1_STATIC_ASSERT(sizeof(PmSecondarySlot) == 0x10C, pm_secondary_slot_size);

extern PmPrimarySlot *g_PmSlotTable;
extern PmSecondarySlot *g_PmSlotTable2;
extern u32 g_PmSlotBuffer[];
int Pm_Stop(int slot, void *owner, int mode);
int Scene_FreeEntityTable(void *owner);

#endif
