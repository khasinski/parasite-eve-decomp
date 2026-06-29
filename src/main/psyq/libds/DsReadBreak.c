/* MASPSX_FLAGS: --stack-return-delay */

extern int g_DsReadBusy;

void DsSyncCallback(int arg0);
void DsReadyCallback(int arg0);
int Render_AllocParticleNode(int com, void *param, void *result, int mode);

void DsReadBreak(void) {
    int *state;

    state = &g_DsReadBusy;
    asm volatile("" : "=r"(state) : "0"(state));
    if (*state == 1) {
        DsSyncCallback(state[-3]);
        DsReadyCallback(state[-2]);
        Render_AllocParticleNode(9, 0, 0, -1);
    }
    *state = 0;
}
