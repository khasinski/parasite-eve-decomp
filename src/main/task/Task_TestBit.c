int Task_TestBit(int **arg0) {
    *arg0[2] = *arg0[0] & (1 << *arg0[1]);
    return 1;
}
