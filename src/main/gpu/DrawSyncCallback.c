/* MASPSX_FLAGS: --stack-return-delay */

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *msg, int arg0);
extern char D_80011854[];

int DrawSyncCallback(int arg0) {
    unsigned char *base;
    register int saved asm("$17");
    int old;

    base = g_GraphDebug;
    saved = arg0;
    if (base[0] >= 2) {
        g_GpuDebugPrintf[0](D_80011854, saved);
    }

    old = *(int *)(base + 0xA);
    *(int *)(base + 0xA) = saved;
    return old;
}
