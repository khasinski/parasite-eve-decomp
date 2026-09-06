/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
extern unsigned int D_80095874;
extern volatile unsigned int D_80095878;
extern unsigned int *D_80095860, *D_80095854;
void Gpu_ResetDmaWaitTimer(void);
int Gpu_SetDisplayBuffer(void);
int Gpu_DmaTimeoutCheck(void);

static __inline__ unsigned int readGpuStatus(void) {
    unsigned int status = *D_80095854;
    asm volatile("" : "+r"(status));
    return status;
}

int Gpu_DrainDmaQueue(int mode) {
    int queued;
    if (mode == 0) {
        Gpu_ResetDmaWaitTimer();
        goto checkQueue;
    retryQueue:
        Gpu_SetDisplayBuffer();
        if (Gpu_DmaTimeoutCheck()) return -1;
    checkQueue:
        if (D_80095874 != D_80095878) goto retryQueue;
        goto checkHardware;
    retryHardware:
        if (Gpu_DmaTimeoutCheck()) return -1;
    checkHardware:
        if (*D_80095860 & 0x01000000) goto retryHardware;
        if (!(readGpuStatus() & 0x04000000)) goto retryHardware;
        return 0;
    }
    queued = (D_80095874 - D_80095878) & 63;
    if (queued) Gpu_SetDisplayBuffer();
    if ((*D_80095860 & 0x01000000) || !(readGpuStatus() & 0x04000000)) {
        return queued ? queued : 1;
    }
    return queued;
}
