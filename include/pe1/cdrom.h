#ifndef PE1_CDROM_H
#define PE1_CDROM_H

int cd_rom4(unsigned char command, void *param, void *result);
int func_80080DC4(unsigned char command, void *param, void *result);
int Cd_GetReadyStatus(void);
int CdRom_GetPendingReadCount(void);

#endif
