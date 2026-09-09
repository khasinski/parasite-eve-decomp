/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_cd.h"

extern int g_DsStreamNoLocFlag;

int Render_BuildParticleFrame(int arg0, CdlLOC *arg1, int arg2, void *arg3, int arg4);
void data_ready_callback(void);
void CdRom_BreakSyncCallback(u_char event, u_char *result);

int DsRead2(CdlLOC *pos, int mode) {
    DsCallback saved_data;
    DsEventCallback saved_sync;
    int ret;

    if (mode & 0x100) {
        asm volatile(
            ".set\tnoat\n\t"
            ".set\tnoreorder\n\t"
            "andi\t$2,%0,0x20\n\t"
            "beqz\t$2,1f\n\t"
            "addiu\t$2,$zero,1\n\t"
            "lui\t$1,%%hi(g_DsStreamNoLocFlag)\n\t"
            "j\t2f\n\t"
            "sw\t$zero,%%lo(g_DsStreamNoLocFlag)($1)\n"
            "1:\n\t"
            "lui\t$1,%%hi(g_DsStreamNoLocFlag)\n\t"
            "sw\t$2,%%lo(g_DsStreamNoLocFlag)($1)\n"
            "2:\n\t"
            ".set\treorder\n\t"
            ".set\tat"
            :
            : "r"(mode)
            : "$2", "$1", "memory");
        saved_data = DsDataCallback(data_ready_callback);
        saved_sync = DsSyncCallback(CdRom_BreakSyncCallback);
        ret = Render_BuildParticleFrame(mode & 0xFF, pos, 0x1B, 0, -1);
        if (ret == 0) {
            DsDataCallback(saved_data);
            DsSyncCallback(saved_sync);
            return 0;
        }
        return ret;
    }

    return Render_BuildParticleFrame(mode & 0xFF, pos, 0x1B, 0, -1);
}
