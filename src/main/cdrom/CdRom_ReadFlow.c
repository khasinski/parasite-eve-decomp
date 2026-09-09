
int CdRom_ReadSectors(int lba, int offset, int dst, int size);

void CdRom_ReadSectorsFromLba(int arg0, int arg1, int arg2) {
    CdRom_ReadSectors(arg0, 0, arg1, arg2);
}
#include "pe1/psyq_cd.h"

int Cd_GetReadyStatus(void);
int CdRom_GetPendingReadCount(void);
int CdRom_GetDiskType(void);
void exit(int code);
CdlLOC *CdIntToPos(int i, CdlLOC *p);
int CdRom_StartRead(CdlLOC *loc, int size, int dst, int mode);
int printf(char *fmt, ...);

extern int g_GameState;
extern u_short g_CdDiskType;
extern char D_8001136C[];

int CdRom_ReadSectors(int lba, int offset, int dst, int size) {
    register int base;
    register int rel;
    register int dst_reg;
    register int size_reg;
    int *state;
    CdlLOC loc;
    int ret;

    base = lba;
    rel = offset;
    dst_reg = dst;
    size_reg = size;
    state = &g_GameState;

    if ((*state & 0x1000000) != 0) {
        return -1;
    }
    if (Cd_GetReadyStatus() != 1) {
        return -1;
    }
    if (CdRom_GetPendingReadCount() != 0) {
        return -1;
    }
    if (CdRom_GetDiskType() != g_CdDiskType) {
        exit(1);
    }

    *state |= 0x1004000;
    base += rel;
    CdIntToPos(base, &loc);
    ret = CdRom_StartRead(&loc, size_reg, dst_reg, 0x80);
    if (ret != 0) {
        return ret;
    }

    *state &= 0xFEFFBFFF;
    printf(D_8001136C, base, size_reg);
    return -1;
}
int Sys_VSyncTimeout(int arg0);

extern int g_GameState;

int CdRom_PollReady(void) {
    int scratch;
    int status;
    int *state;

    status = Sys_VSyncTimeout((int)&scratch);
    if ((unsigned int)(status + 1) < 2U) {
        state = &g_GameState;
        *state &= 0xFEFFBFFF;
    }
    return status;
}
