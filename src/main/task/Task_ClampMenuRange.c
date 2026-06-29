void Menu_ClampRange(int arg0);

int Task_ClampMenuRange(int **arg0) {
    Menu_ClampRange(*arg0[0]);
    return 1;
}
