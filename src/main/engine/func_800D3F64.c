extern void *D_800F32D0;

void Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);

void func_800D3F64(int arg0, int arg1) {
    int **ctx;
    register int *child asm("$3");
    register volatile int *node asm("$2");
    volatile short sx;
    volatile short sy;
    volatile short sz;
    int x;
    int y;
    int z;

    ctx = D_800F32D0;
    child = ctx[2];
    node = (volatile int *)child[0x8E];
    x = node[5];
    asm volatile("nop");
    sx = x;
    asm volatile("" ::: "memory");
    x = (short)x;
    node = (volatile int *)child[0x8E];
    y = node[6];
    sy = y;
    asm volatile("" ::: "memory");
    y = (short)y;
    node = (volatile int *)child[0x8E];
    z = node[7];
    sz = z;
    z = (short)z;
    Asset_Find08w(arg0, arg1, x, y, z);
}
