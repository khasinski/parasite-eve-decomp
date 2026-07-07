/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_cd.h"

int CdPosToInt_Local(CdlLOC *p);
CdlLOC *CdIntToPos_Local(int i, CdlLOC *p);

extern int g_DsStreamNoLocFlag;
extern CdlLOC D_800A3490;
extern int D_800A3494;

int StGetBackloc(CdlLOC *arg0) {
    register CdlLOC *saved_arg asm("$16") = arg0;

    if (g_DsStreamNoLocFlag != 0) {
        return -1;
    }

    {
        register int pos asm("$2");

        /* Match note: D_800A3490 low half is in the CdPosToInt_Local delay slot. */
        asm volatile(
            ".set\tnoreorder\n\t"
            "lui\t$4,%%hi(D_800A3490)\n\t"
            "jal\tCdPosToInt_Local\n\t"
            "addiu\t$4,$4,%%lo(D_800A3490)\n\t"
            ".set\treorder"
            : "=r"(pos)
            :
            : "$4", "$31", "memory");
        CdIntToPos_Local(pos + 1, saved_arg);
    }
    return D_800A3494;
}
