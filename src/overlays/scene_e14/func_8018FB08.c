typedef signed short s16;
typedef unsigned short u16;

void func_8018FB08(void *arg0, char *arg1, char *arg2) {
    u16 phase_y;

    *(u16 *)(arg2 + 0x10) += 0xF;
    if (*(s16 *)(arg1 + 0x2) < 0x10) {
        phase_y = *(volatile u16 *)(arg2 + 0x12);
        phase_y += 8;
    } else {
        phase_y = *(volatile u16 *)(arg2 + 0x12);
    }
    *(u16 *)(arg2 + 0x12) = phase_y - 4;
    *(u16 *)(arg2 + 0xA) += 0x64;
    if (*(s16 *)(arg2 + 0x12) <= 0) {
        arg1[1] = 2;
    }
}
