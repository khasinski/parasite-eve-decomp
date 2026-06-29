extern unsigned char g_MemCardPortStates[];

int close(int fd);

void MemCard_CloseAll(void) {
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
}
