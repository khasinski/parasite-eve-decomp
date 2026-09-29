#ifndef PE1_CDROM_RUNTIME_H
#define PE1_CDROM_RUNTIME_H

#include "common.h"

struct CdlLOC;

extern u8 *g_StrFileDirBuffer;

int Sys_VSyncTimeout(void *argument);
int CdRom_SendQueuedCmd(u8 *destination);
int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size);
int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size);
int CdRom_StartRead(struct CdlLOC *position, int sectors, void *destination,
                    int mode);

#endif
