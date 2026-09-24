#ifndef PE1_AKAO_H
#define PE1_AKAO_H

#include "pe1/akao/track.h"
#include "pe1/akao/queue.h"
#include "pe1/akao/commands.h"
#include "pe1/akao/spu_voice.h"

/*
 * Track structs are still intentionally partial in C files. The FF7/FF8 AKAO
 * cross-reference has stable field roles, but PE1 offsets do not fully match
 * either game yet.
 */

/* Sample transfer and stream instrument relocation. Addresses passed to the
 * upload wrapper are 32-bit main-memory addresses in the PSX ABI. */
int Spu_ValidateSampleHeader(void *header);
int Spu_WriteRegChecked(int address);
void Spu_UploadWithPrepare(int source, int size);
void Spu_RebaseStreamAddrs(unsigned char *instruments, int address, int count);
int Spu_UploadStreamBlockA(unsigned bank, unsigned char *cursor, unsigned size);
int Spu_UploadStreamBlockB(unsigned bank, unsigned char *cursor);
/* Stream instrument scratch table; bank selection advances by 0x400 bytes. */
extern unsigned D_800B4900[];
extern unsigned D_800B4D00[];
extern unsigned D_8009D270, D_8009D1EC, D_8009D204;

extern void * volatile g_AkaoBgmHandle;
int Akao_SendTableCommand(void *table, int command, int arg1, int arg2, int arg3);

#endif
