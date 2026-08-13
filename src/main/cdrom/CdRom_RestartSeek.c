
#include "pe1/psyq_cd.h"

void DsSyncCallback(void *arg0);
CdlLOC *CdRom_GetCurrentPosPtr(void);
int CdPosToInt(CdlLOC *arg0);
int CdRom_GetCmdMode(void);
int CdRom_GetCmdParam(void);
int Render_BuildParticleFrame(int arg0, int arg1, int arg2, void *arg3, int arg4);
void CdRom_SeekDoneCallback(void);

typedef struct CdRestartSeekPage {
    int position;
    char reserved04[0x4910];
} CdRestartSeekPage;

register CdRestartSeekPage *g_CdRestartSeekPage asm("$1");

int CdRom_RestartSeek(void) {
    int temp_s0;
    int temp_s1;
    int temp_v0;
    int cmd_mode;

    DsSyncCallback(0);
    temp_v0 = CdPosToInt(CdRom_GetCurrentPosPtr());
    g_CdRestartSeekPage = (CdRestartSeekPage *)0x800A0000;
    g_CdRestartSeekPage[-1].position = temp_v0;
    cmd_mode = CdRom_GetCmdMode();
    temp_s0 = cmd_mode & 0xFF;
    temp_s1 = (int)CdRom_GetCurrentPosPtr();
    temp_v0 = CdRom_GetCmdParam() & 0xFF;
    {
        int arg0 = temp_s0;
        int neg_one;

        neg_one = -1;
        return Render_BuildParticleFrame(arg0, temp_s1, temp_v0, CdRom_SeekDoneCallback, neg_one);
    }
}
