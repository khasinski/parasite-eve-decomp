/* Experimental full C reconstruction; not an exact match. See README.md. */
/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_cd.h"

extern int g_DsStreamNoLocFlag;

int Render_BuildParticleFrame(int arg0, CdlLOC *arg1, int arg2, void *arg3, int arg4);
void data_ready_callback(void);
void CdRom_BreakSyncCallback(u_char event, u_char *result);

int DsRead2(CdlLOC *pos, int mode) {
    DsCallback saved_data;
    register int saved_mode asm("$16");
    DsEventCallback saved_sync;
    register CdlLOC *saved_pos asm("$18");
    int ret;

    saved_pos = pos;
    saved_mode = mode;
    if (saved_mode & 0x100) {
        {
            int one = 1;
            if (saved_mode & 0x20) {
                g_DsStreamNoLocFlag = 0;
            } else {
                g_DsStreamNoLocFlag = one;
                asm("" : : "r"(one));
            }
        }
        saved_data = DsDataCallback(data_ready_callback);
        saved_sync = DsSyncCallback(CdRom_BreakSyncCallback);
        ret = Render_BuildParticleFrame(saved_mode & 0xFF, saved_pos, 0x1B, 0, -1);
        if (ret == 0) {
            DsDataCallback(saved_data);
            DsSyncCallback(saved_sync);
            return 0;
        }
        return ret;
    }

    return Render_BuildParticleFrame(saved_mode & 0xFF, saved_pos, 0x1B, 0, -1);
}
