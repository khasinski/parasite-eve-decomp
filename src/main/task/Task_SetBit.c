int Task_SetBit(int **arg0) {
    *arg0[0] |= 1 << *arg0[1];
    return 1;
}
