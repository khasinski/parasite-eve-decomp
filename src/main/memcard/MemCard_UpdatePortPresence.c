/* MASPSX_FLAGS: --stack-return-delay */

extern int g_MemCardPort2Present;
extern int g_MemCardPort1Present;
extern int g_MemCardServiceReady;
extern int g_MemCardPort1DebounceTimer;
extern int g_MemCardPort2DebounceTimer;
extern void *g_MemCardObjArray;
extern void (*g_MemCardObjResetFn)(int);

int MemCard_UpdatePortPresence(int arg0) {
    int arg;
    register int *timer_a asm("$18");
    register int timer_value asm("$2");
    int bit2;
    int old;
    int *timer;

    arg = arg0;
    old = (g_MemCardPort2Present << 1) | ((unsigned int)g_MemCardPort1Present < 1);
    if (old != arg) {
        g_MemCardServiceReady = 0;
        if ((arg & 1) != 0) {
            timer_a = &g_MemCardPort1DebounceTimer;
            timer_value = *timer_a;
            g_MemCardPort1Present = 0;
            if (timer_value >= 150) {
                g_MemCardObjResetFn((int)g_MemCardObjArray);
            }
            *timer_a = 0;
        } else {
            g_MemCardPort1Present = 1;
        }
        bit2 = arg & 2;
        if (bit2 != 0) {
            timer = &g_MemCardPort2DebounceTimer;
            timer_value = *timer;
            g_MemCardPort2Present = 1;
            if (timer_value >= 150) {
                g_MemCardObjResetFn((int)g_MemCardObjArray + 0xF0);
            }
            *timer = 0;
        } else {
            g_MemCardPort2Present = 0;
        }
        g_MemCardServiceReady = 1;
    }
    return old;
}
