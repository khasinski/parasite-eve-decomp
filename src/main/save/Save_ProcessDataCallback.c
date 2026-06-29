extern void CdRom_ResetFileDescriptors(void);
/* MASPSX_FLAGS: --stack-return-delay */
extern void DS_read_cbready(void);
extern void DsDataCallback(void *callback);
extern void CdRom_IssueSetMode(int arg0, int arg1);
extern void Render_AllocParticleNode(int arg0, void *arg1, void *arg2, int arg3);

extern int g_CdReadInProgress;

asm(".globl func_80081268");
asm("func_80081268 = Save_ProcessDataCallback");

void Save_ProcessDataCallback(void) {
    int *state;

    state = &g_CdReadInProgress;
    asm volatile("" : "=r"(state) : "0"(state));
    if (*state == 1) {
        CdRom_ResetFileDescriptors();
        DS_read_cbready();
        if (state[-5] & 1) {
            DsDataCallback((void *)state[-3]);
        }
        CdRom_IssueSetMode(1, 0);
        Render_AllocParticleNode(9, 0, 0, -1);
    }
    g_CdReadInProgress = 0;
}
