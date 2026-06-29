/* MASPSX_FLAGS: --stack-return-delay */
extern int g_CdEnqueueIndex;
extern unsigned int g_CdEnqueueCmdQueue[];
extern unsigned char g_CdEnqueueCmdArgs[];

void Util_Copy8(unsigned char *dst, unsigned char *src);

void CdRom_EnqueueCmd(void *arg0, unsigned char arg1, unsigned char *arg2) {
    int *p = &g_CdEnqueueIndex;
    volatile int *vp = p;
    char *base;
    int next;

    *((void **)((char *)g_CdEnqueueCmdQueue + (*vp << 4))) = arg0;
    *((unsigned char *)((char *)g_CdEnqueueCmdArgs + (*vp << 4))) = arg1;

    base = (char *)p - 0x7B;
    Util_Copy8((unsigned char *)(base + (*vp << 4)), arg2);

    next = *p + 1;
    *p = next;
    if (next >= 8) {
        *p = 0;
    }
}
