void func_8018FB04(void *unused, char *state, char *work) {
    *(unsigned short *)(work + 0x10) += 0xF;

    if (*(short *)(state + 2) < 0x10) {
        *(unsigned short *)(work + 0x12) += 8;
    } else {
        *(unsigned short *)(work + 0x12) -= 4;
    }

    *(unsigned short *)(work + 0xA) += 0x64;
    if (*(short *)(work + 0x12) <= 0) {
        state[1] = 2;
    }
}
