extern volatile int D_8009D2E0;

int Spu_PollOrWaitSeqBusy(int mode) {
    if (mode == 0) {
        while (D_8009D2E0 & 1) {
        }
        return 0;
    }

    return D_8009D2E0 & 1;
}
int func_800851A8(void *arg0, int arg1);

void Spu_UploadSampleBlockBlocking(void *arg0, int arg1) {
    while (func_800851A8(arg0, arg1) == 1) {
    }
}
extern int D_8009D24C;

int Spu_GetTransferStatus(void) {
    return D_8009D24C;
}
