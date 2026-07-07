/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 */

typedef unsigned short u16;

extern char *g_CurrentEntity;

int Geo_PointInPoly(int arg0, int arg1, int arg2, int arg3);

int Entity_CallAction(int **arg0) {
    int **args;
    register int *arg0_ptr asm("$2");
    int *arg1_ptr;
    register int *arg2_ptr asm("$7");
    int arg2;
    int arg1;
    int arg3;
    register int base asm("$8");
    register char *current asm("$2");
    int result;

    args = arg0;
    arg0_ptr = args[3];
    arg1_ptr = args[1];
    asm volatile("" : "=r"(arg1_ptr) : "0"(arg1_ptr));
    arg2_ptr = args[2];
    arg2 = *arg0_ptr;
    current = g_CurrentEntity;
    asm volatile("" ::: "memory");
    arg1 = *(volatile int *)arg1_ptr;
    asm volatile("" : "=r"(arg1) : "0"(arg1));
    arg3 = *(u16 *)arg2_ptr;
    base = *(int *)(current + 0x9C);
    arg0_ptr = args[0];
    arg2 <<= 1;
    result = Geo_PointInPoly(*arg0_ptr, arg1, base + arg2, arg3);
    *args[4] = result;
    return 1;
}
