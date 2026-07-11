extern int D_8018F18C;

void func_8018F168(unsigned char *arg0) {
    unsigned char *ptr = *(unsigned char **)(arg0 + 8);

    if (ptr[0xE] == 0x10) {
        *(int **)(arg0 + 0xC) = &D_8018F18C;
    }
}
