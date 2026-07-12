/* MASPSX_FLAGS: --store-call-delay */

typedef signed short s16;

int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);
int func_800C6B20(void *arg0);

static inline int div4096(int value) {
    if (value < 0) {
        value += 0xFFF;
    }
    return value >> 12;
}

int func_800CEB8C(s16 *a, s16 *b, int radius) {
    /* Match note: target reuses $s0 for the angle and then the x offset. */
    register int angle asm("$16");
    volatile char points[0x20];
    int dx;
    int dz;
    int x;
    register int z asm("$6");

    dx = b[0] - a[0];
    dz = b[2] - a[2];
    angle = -Gte_Atan2(dz, dx);
    x = div4096(rsin(angle) * radius);
    z = div4096(rcos(angle) * radius);

    *(volatile s16 *)(points + 0x00) = a[0] - x;
    *(volatile s16 *)(points + 0x04) = a[2] - z;
    *(volatile s16 *)(points + 0x08) = a[0] + x;
    /* Match note: keep stack point writes scalar and in target load-delay order. */
    asm volatile("");
    *(volatile s16 *)(points + 0x0C) = a[2] + z;
    *(volatile s16 *)(points + 0x10) = b[0] - x;
    asm volatile("");
    *(volatile s16 *)(points + 0x14) = b[2] - z;
    *(volatile s16 *)(points + 0x18) = b[0] + x;
    asm volatile("");
    *(volatile s16 *)(points + 0x1C) = b[2] + z;

    return func_800C6B20((void *)points);
}
