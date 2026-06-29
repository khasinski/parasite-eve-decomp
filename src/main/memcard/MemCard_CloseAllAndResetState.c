extern unsigned char g_MemCardPortStates[];
extern unsigned char D_800A12ED;
extern unsigned char D_800A0ED5;
extern int g_McOpPending;

int close(int fd);

void MemCard_CloseAllAndResetState(void) {
    unsigned char *base;
    unsigned char *state;
    unsigned char *end;
    int stateOpen;
    int stateBusy;
    int closedFd;
    int stateClosed;

    base = g_MemCardPortStates;
    if (base < base + 0x830) {
        stateOpen = 8;
        stateBusy = 10;
        closedFd = -1;
        stateClosed = 12;
        state = base + 1;
        end = base + 0x831;
        do {
            if ((*state == stateOpen) || (*state == stateBusy)) {
                close(*(volatile int *)(state + 0xB));
                *(volatile int *)(state + 0xB) = closedFd;
                *state = stateClosed;
            }
            state += 0x418;
        } while (state < end);
    }

    D_800A12ED = 0;
    D_800A0ED5 = 0;
    g_McOpPending = 0;
}
