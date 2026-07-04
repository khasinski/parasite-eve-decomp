/* MASPSX_FLAGS: --la-call-delay */

extern volatile int g_VSyncCount;
extern char D_800116FC[];

void puts(char *arg0);
void ChangeClearPAD(int arg0);
void ChangeClearRCnt(int arg0, int arg1);

void v_wait(int arg0, int arg1) {
    volatile int timeout = arg1 << 15;

    if (g_VSyncCount < arg0) {
        do {
            if (--timeout == -1) {
                puts(D_800116FC);
                ChangeClearPAD(0);
                ChangeClearRCnt(3, 0);
                return;
            }
        } while (g_VSyncCount < arg0);
    }
}
