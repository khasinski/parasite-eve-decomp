/* MASPSX_FLAGS: --stack-return-delay --store-return-delay */

extern void CdRom_ResetFileDescriptors(void);
extern void DS_read_cbready(void);
extern void DsDataCallback(void *callback);
extern void CdRom_IssueSetMode(int arg0, int arg1) asm("func_8007FCBC");
extern void Render_AllocParticleNode(int arg0, void *arg1, void *arg2, int arg3);

extern int D_8009B6CC;

void Save_ProcessDataCallback(void) {
    int *state;

    state = &D_8009B6CC;
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
    D_8009B6CC = 0;
}
