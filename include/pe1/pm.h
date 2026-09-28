#ifndef PE1_PM_H
#define PE1_PM_H

#include "pe1/pm_types.h"

int Scene_LoadRoomAssets(u32 command, void *owner);

#ifdef PE1_PM_LEGACY_RAW_VIEWS
/* The matching pm2 body retains raw global views; typed cleanup code shares
 * the same linker symbols through these ABI aliases. */
extern PmPrimarySlot *g_PmSlotTableTyped __asm__("g_PmSlotTable");
extern PmSecondarySlot *g_PmSlotTable2Typed __asm__("g_PmSlotTable2");
extern u32 g_PmSlotBufferTyped[] __asm__("g_PmSlotBuffer");
int Pm_Stop(int slot, int owner, int mode);
#else
extern PmPrimarySlot *g_PmSlotTable;
extern PmSecondarySlot *g_PmSlotTable2;
extern u32 g_PmSlotBuffer[];
int Pm_Stop(int slot, void *owner, int mode);
int Scene_FreeEntityTable(void *owner);
#endif

#endif
