int func_800C2B68(void);
void func_800C6C18(void);

void func_8018F2E8(void *arg0, char *event, char *state) {
    register char *state2 asm("$5");

    state2 = state;
    if (*(short *)(state + 0x2A) != 0) {
        *(short *)(state + 0x2A) -= 1;
    }

    if (*(short *)(state2 + 0x2C) == 1) {
        *(short *)(state2 + 0x2C) = 0;
        if (*(short *)(state2 + 0x2A) == 0) {
            *(short *)(state2 + 0x2A) = *(unsigned short *)(state2 + 0x28);
            func_800C6C18();
        }
    }

    if (func_800C2B68() == 1) {
        event[1] = 2;
    }
}
