extern int func_800C6CE0(void);

int func_8018F1BC(char *state) {
    register void *ptr asm("$4");
    register unsigned int value asm("$2");
    register unsigned int mask asm("$3");

    state[0] = 4;
    if ((unsigned int)func_800C6CE0() < 2U) {
        return 0;
    }
    ptr = **(void ***)(state + 8);
    mask = 0xC0FFFFFF;
    value = *(unsigned int *)ptr;
    value &= mask;
    *(unsigned int *)ptr = value;

    return 0;
}
