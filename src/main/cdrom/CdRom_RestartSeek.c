/* MASPSX_FLAGS: --stack-return-delay --store-call-delay */

void DsSyncCallback(void *arg0);
void *CdRom_GetCurrentPosPtr(void);
int CdPosToInt(void *arg0);
int CdRom_GetCmdMode(void);
int CdRom_GetCmdParam(void);
int Render_BuildParticleFrame(int arg0, int arg1, int arg2, void *arg3, int arg4);
void CdRom_SeekDoneCallback(void);

extern int D_8009B6EC;

int CdRom_RestartSeek(void) {
    int temp_s0;
    int temp_s1;
    int temp_v0;

    DsSyncCallback(0);
    D_8009B6EC = CdPosToInt(CdRom_GetCurrentPosPtr());
    temp_s0 = CdRom_GetCmdMode() & 0xFF;
    temp_s1 = (int)CdRom_GetCurrentPosPtr();
    temp_v0 = CdRom_GetCmdParam() & 0xFF;
    return Render_BuildParticleFrame(temp_s0, temp_s1, temp_v0, CdRom_SeekDoneCallback, -1);
}
