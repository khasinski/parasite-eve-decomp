extern unsigned char D_800A0EDC[];
extern unsigned char D_800A12F4;
extern int g_MemCardConnectDebounce;
extern int D_800A1848[];

int MemCard_PollTransferDelay(void) {
    int offset;
    int *pending;
    int count;
    int one;
    int reset;
    int value;
    int clamped;
    register int final_value asm("$4");

    count = 0;
    reset = 12;
    one = 1;
    pending = D_800A1848;
    value = g_MemCardConnectDebounce;
    offset = 0;
    g_MemCardConnectDebounce = value - (value > 0);

    do {
        if ((unsigned int)(D_800A0EDC[offset] - 2) < 2) {
            g_MemCardConnectDebounce = reset;
            *pending = one;
        }
        pending++;
        count++;
        offset += 0x418;
    } while (count < 2);

    pending = D_800A1848;
    __asm__ volatile("" : "=r"(pending) : "0"(pending));
    if (pending[0] != 0) {
        if (D_800A0EDC[0] != 4) {
            goto done;
        }
    }
    if (*(pending + 1) != 0) {
        if (D_800A12F4 != 4) {
            goto done;
        }
    }

    final_value = g_MemCardConnectDebounce;
    clamped = 4;
    if (final_value < 5) {
        clamped = final_value;
    }
    g_MemCardConnectDebounce = clamped;
    *(pending + 1) = 0;
    pending[0] = 0;
done:
    return g_MemCardConnectDebounce > 0;
}
