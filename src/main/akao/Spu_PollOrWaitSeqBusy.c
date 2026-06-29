extern volatile int D_8009D2E0;

int Spu_PollOrWaitSeqBusy(int arg0) {
    if (arg0 == 0) {
        while (D_8009D2E0 & 1) {
        }

        return 0;
    } else {
        return D_8009D2E0 & 1;
    }
}
