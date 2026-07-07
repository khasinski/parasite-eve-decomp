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
                /* Match note: D_800116FC low half is in the puts delay slot. */
                asm volatile(
                    ".set\tnoreorder\n\t"
                    "lui\t$4,%%hi(D_800116FC)\n\t"
                    "jal\tputs\n\t"
                    "addiu\t$4,$4,%%lo(D_800116FC)\n\t"
                    ".set\treorder"
                    :
                    :
                    : "$4", "$31", "memory");
                ChangeClearPAD(0);
                ChangeClearRCnt(3, 0);
                return;
            }
        } while (g_VSyncCount < arg0);
    }
}
