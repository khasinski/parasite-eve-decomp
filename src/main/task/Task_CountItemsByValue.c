int Inv_CountByValue(int arg0);

int Task_CountItemsByValue(int **arg0) {
    *arg0[1] = Inv_CountByValue(*arg0[0]);
    return 1;
}
