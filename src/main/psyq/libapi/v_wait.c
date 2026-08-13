/* CC1_FLAGS: -fno-expensive-optimizations */
extern volatile int g_VSyncCount;
extern char D_800116FC[];
register volatile int g_VWaitArg asm("$4");

void puts(char *arg0);
void ChangeClearPAD(int arg0);
void ChangeClearRCnt(int arg0, int arg1);

void v_wait(int arg0, int arg1) {
    volatile int timeout = arg1 << 15;

    if (g_VSyncCount < g_VWaitArg) {
        do {
            if (--timeout == -1) {
                g_VWaitArg = 0x80010000;
                g_VWaitArg += 0x16FC;
                puts((char *)g_VWaitArg);
                ChangeClearPAD(0);
                ChangeClearRCnt(3, 0);
                return;
            }
        } while (g_VSyncCount < g_VWaitArg);
    }
}
