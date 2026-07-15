extern int *D_8009B4B4;

int func_8007E160(void) {
    register int *state asm("$3");
    register int result asm("$2");

    state = D_8009B4B4;
    result = state[1] & 1;
    if (result == 0) {
        result = 0;
        goto done;
    }

    result = state[0] & 1;
    if (result != 0) {
        result = 1;
        goto done;
    }
    result = 0;

done:
    return result;
}
