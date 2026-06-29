void Tbl_ClearEntry(int arg0);

int Task_ClearTableEntry(short **arg0) {
    Tbl_ClearEntry(*arg0[0]);
    return 1;
}
