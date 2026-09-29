#ifndef PE1_CDROM_H
#define PE1_CDROM_H

#include "common.h"

struct CdlLOC;

int cd_rom4(unsigned char command, void *param, void *result);
int func_80080DC4(unsigned char command, void *param, void *result);
int Cd_GetReadyStatus(void);
int CdRom_GetPendingReadCount(void);
int CdRom_PollReady(void);
int Sys_VSyncTimeout(void *argument);
int CdRom_SendQueuedCmd(u8 *destination);
int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size);
int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size);
int CdRom_StartRead(struct CdlLOC *position, int sectors, void *destination,
                    int mode);

#endif
