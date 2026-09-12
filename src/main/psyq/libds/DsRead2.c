/* ASSEMBLER: GNU */

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
        {
            register int one asm("$2");
            int mask = mode & 0x20;

            if (mask) {
                g_DsStreamNoLocFlag = 0;
            } else {
                one = 1;
                g_DsStreamNoLocFlag = one;
                /* Keep 1 in v0 through the store and the branch delay slot. */
                asm("" : : "r"(one));
            }
        }
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
