/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_cd.h"

CdlLOC *CdIntToPos(int i, CdlLOC *p);
int CdRom_StartRead(CdlLOC *loc, int arg1, int arg2, int mode);
int Sys_VSyncTimeout(int arg0);

int ds_read(int arg0, int arg1, int arg2) {
    CdlLOC loc;
    int status;

    CdIntToPos(arg1, &loc);
    CdRom_StartRead(&loc, arg0, arg2, 0x80);

    do {
        status = Sys_VSyncTimeout(0);
    } while (status > 0);

    return status < 1U;
}
