extern volatile int D_8009D2E0;

int Spu_PollOrWaitSeqBusy(int mode) {
    if (mode == 0) {
        while (D_8009D2E0 & 1) {
        }
        return 0;
    }

    return D_8009D2E0 & 1;
}
