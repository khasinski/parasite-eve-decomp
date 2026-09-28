#ifndef PE1_PM_H
#define PE1_PM_H

#include "pe1/pm_types.h"

int Scene_LoadRoomAssets(u32 command, void *owner);
extern PmAuxiliaryPointerTable g_PmAuxiliaryPointerTable
    __asm__("D_800E10A0");

#ifdef PE1_PM_LEGACY_RAW_VIEWS
/* A few exact-match paths retain raw global views; typed operations use aliases
 * for the same linker symbols where the retail addressing permits it. */
extern PmPrimarySlot *g_PmSlotTableTyped __asm__("g_PmSlotTable");
extern PmSecondarySlot *g_PmSlotTable2Typed __asm__("g_PmSlotTable2");
extern u32 g_PmSlotBufferTyped[] __asm__("g_PmSlotBuffer");
extern PmPrimarySlot *g_PmSlotTable;
extern PmSecondarySlot *g_PmSlotTable2;
extern u32 g_PmSlotBuffer[];
extern PmCommand **g_PmCmdHandlerTable;
extern char *g_PmSlotTableRaw __asm__("g_PmSlotTable");
extern char *g_PmSlotTable2Raw __asm__("g_PmSlotTable2");
int Pm_Stop(int slot, int owner, int mode);
#else
extern PmPrimarySlot *g_PmSlotTable;
extern PmSecondarySlot *g_PmSlotTable2;
extern u32 g_PmSlotBuffer[];
extern PmCommand **g_PmCmdHandlerTable;
int Pm_Stop(int slot, void *owner, int mode);
int Scene_FreeEntityTable(void *owner);
#endif

#endif
