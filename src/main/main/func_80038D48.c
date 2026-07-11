extern int D_80091A24;

int func_80038D48(void) {
    int *state = &D_80091A24;

    if (*state != 0) {
        *state = 0;
        return 0;
    }

    return 0xFF;
}
