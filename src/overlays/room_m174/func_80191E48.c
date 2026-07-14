void func_80191E48(void *arg0, char *state, char *entry) {
    *(unsigned short *)(entry + 0x12) = *(unsigned short *)(entry + 0x12) - 8;
    *(unsigned short *)(entry + 0x10) = *(unsigned short *)(entry + 0x10) + 0x1E;
    *(unsigned short *)(entry + 2) = *(unsigned short *)(entry + 2) - 0xF;

    if (*(short *)(entry + 0x12) < 0) {
        *(unsigned short *)(entry + 0x12) = 0;
        state[1] = 2;
    }
}
