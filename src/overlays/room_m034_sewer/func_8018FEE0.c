void func_8018FEE0(void *unused, char *state, signed char *work) {
    work[2]++;

    if (work[2] >= 0x15) {
        state[1] = 2;
    }

    if (*(short *)(work + 4) >= 9) {
        *(short *)(work + 4) -= 8;
    }
}
