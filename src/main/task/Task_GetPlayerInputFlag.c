extern int D_8009D2E8;

int Task_GetPlayerInputFlag(int **args) {
    if (D_8009D2E8 & 1) {
        *args[0] = 0;
    } else {
        *args[0] = 1;
    }

    return 1;
}
