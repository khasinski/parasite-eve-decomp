#ifndef PE1_CDROM_H
#define PE1_CDROM_H

#include "common.h"

struct CdlLOC;

int DsControl(unsigned char command, void *param, void *result);
int DsControlB(unsigned char command, void *param, void *result);
int DsSystemStatus(void);
int DsShellOpen(void);
int DsQueueLen(void);
void DS_restart(void);
int DS_system_active(void);
int DsReset(void);
int ER_retry(void);
int DS_lastcom(void);
int DS_lastmode(void);
struct CdlLOC *DS_lastpos(void);
struct CdlLOC *DsLastPos(struct CdlLOC *destination);
int CdRom_PollReady(void);
int DsReadSync(void *argument);
int DsReady(u8 *destination);
int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size);
int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size);
int DsRead(struct CdlLOC *position, int sectors, void *destination,
                    int mode);

#endif
