/* MASPSX_FLAGS: --stack-return-delay */

extern int D_8009B708;

void DsSyncCallback(int arg0);
void DsReadyCallback(int arg0);
int CdRom_GetPendingReadCount(void);
int CdRom_RestartSeek(void);

void CdRom_ReadDoneCallback(unsigned char arg0, void *arg1) {
    int *state;
    int status;
    register void *data asm("$18");
    void (*callback)(int, void *, void *);

    state = &D_8009B708;
    asm volatile("" : "=r"(state) : "0"(state));
    data = arg1;
    if (state[0] == 0) {
        return;
    }
    if (state[1] == 0) {
        return;
    }

    status = arg0;
    if (status == 2) {
        if (CdRom_GetPendingReadCount() == 0) {
            CdRom_RestartSeek();
        }
        return;
    }

    DsSyncCallback(state[-2]);
    DsReadyCallback(state[-1]);
    callback = (void *)state[-5];
    state[1] = 0;
    if (callback != 0) {
        callback(status, data, 0);
    }
}
