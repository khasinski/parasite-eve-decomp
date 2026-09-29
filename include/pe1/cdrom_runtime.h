#ifndef PE1_CDROM_RUNTIME_H
#define PE1_CDROM_RUNTIME_H

#include "common.h"

int Sys_VSyncTimeout(void *argument);
int CdRom_SendQueuedCmd(u8 *destination);

#endif
