extern void CdRom_AbortCmd(void);
extern void DsReadBreak(void);
extern void CdRom_EnableDsReadSystem(void);

extern int g_CdDsReadQueueState;
extern int g_CdDsReadIndex;
extern int g_CdPendingReadCount;
extern unsigned char g_CdDsReadQueue[];

void CdRom_ResetFileDescriptors(void) {
    int i;
    register unsigned char *p asm("$4");
    int j;
    unsigned char *q;

    CdRom_AbortCmd();

    i = 0;
    g_CdPendingReadCount = 0;
    g_CdDsReadIndex = 0;
    g_CdDsReadQueueState = 0;
    p = g_CdDsReadQueue;

    while (i < 8) {
        j = 3;
        q = p + 3;
        *(int *)p = 0;
        p[4] = 0;
        for (; j >= 0; j--, q--) {
            q[5] = 0;
        }
        *(int *)(p + 0xC) = 0;
        *(int *)(p + 0x10) = 0;
        *(int *)(p + 0x14) = 0;
        asm volatile("" : "=r"(i) : "0"(i));
        i++;
        p += 0x18;
    }

    DsReadBreak();
    CdRom_EnableDsReadSystem();
}
