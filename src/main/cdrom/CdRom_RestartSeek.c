/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_cd.h"

void DsSyncCallback(void *arg0);
CdlLOC *CdRom_GetCurrentPosPtr(void);
int CdPosToInt(CdlLOC *arg0);
int CdRom_GetCmdMode(void);
int CdRom_GetCmdParam(void);
int Render_BuildParticleFrame(int arg0, int arg1, int arg2, void *arg3, int arg4);
void CdRom_SeekDoneCallback(void);

extern int D_8009B6EC;

int CdRom_RestartSeek(void) {
    register int temp_s0 asm("$16");
    register int temp_s1 asm("$17");
    int temp_v0;
    register int cmd_mode asm("$2");

    DsSyncCallback(0);
    temp_v0 = CdPosToInt(CdRom_GetCurrentPosPtr());
    /* Match note: D_8009B6EC store is in the CdRom_GetCmdMode delay slot. */
    asm volatile(
        ".set\tnoreorder\n\t"
        ".set\tnoat\n\t"
        "lui\t$at,%%hi(D_8009B6EC)\n\t"
        "jal\tCdRom_GetCmdMode\n\t"
        "sw\t%1,%%lo(D_8009B6EC)($at)\n\t"
        ".set\tat\n\t"
        ".set\treorder"
        : "=r"(cmd_mode)
        : "r"(temp_v0)
        : "$31", "memory");
    temp_s0 = cmd_mode & 0xFF;
    temp_s1 = (int)CdRom_GetCurrentPosPtr();
    temp_v0 = CdRom_GetCmdParam() & 0xFF;
    {
        register int arg0 asm("$4") = temp_s0;
        register int neg_one asm("$3");

        neg_one = -1;
        return Render_BuildParticleFrame(arg0, temp_s1, temp_v0, CdRom_SeekDoneCallback, neg_one);
    }
}
