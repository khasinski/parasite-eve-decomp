extern void *D_800F32D0;

int func_800D3FD8(void) {
    int **ctx;
    int *node;
    int value;

    ctx = D_800F32D0;
    node = (int *)ctx[2][0];
    value = 0x80;
    if (node != 0) {
        value = node[2];
        if (value >= 0x41) {
            value = 0x80;
        }
    }
    return value;
}
