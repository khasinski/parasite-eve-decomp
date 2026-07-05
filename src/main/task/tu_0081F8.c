

int Task_ClearBit(int **arg0) {
    *arg0[0] &= ~(1 << *arg0[1]);
    return 1;
}

int Task_TestBit(int **arg0) {
    *arg0[2] = *arg0[0] & (1 << *arg0[1]);
    return 1;
}

int Task_SetBit(int **arg0) {
    *arg0[0] |= 1 << *arg0[1];
    return 1;
}
