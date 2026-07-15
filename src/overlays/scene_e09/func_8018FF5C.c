void func_8018FF5C(void *arg0, char *event, char *state) {
    if (*(short *)(event + 2) < 0x10) {
        *(short *)(state + 0x10) += 0xF0;
        if (*(short *)(event + 2) < 0x10) {
            *(short *)(state + 0x12) -= 8;
        }
    }

    if (*(short *)(event + 2) == 0x10) {
        event[1] = 2;
    }
}
