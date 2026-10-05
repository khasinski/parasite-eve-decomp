void func_800C4E50(void *arg0);

void func_8018F718(void *ent, void *unused, unsigned char *state) {
    *(short *)(state + 0x120) = 0x5DC;
    *(short *)(state + 0x122) = 0xFF;
    *(short *)(state + 0x114) = 0x10;
    state[0x10C] = 0x40;
    state[0x10D] = 0x20;
    state[0x10E] = 0x10;
    state[0x110] = 0x80;
    state[0x111] = 0x80;
    state[0x112] = 0x80;
    *(short *)(state + 0x116) = 0xF0;
    *(short *)(state + 0x118) = 0xA;
    *(short *)(state + 0x124) = 0;
    *(short *)(state + 0x11A) = 0;
    *(void **)(state + 0x108) = state + 8;
    func_800C4E50(state + 0x108);
}
