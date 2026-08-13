
/* CC1_FLAGS: -fno-schedule-insns */

register int *g_TaskGpuReg2 asm("$10");

void Task_InitGpuHwRegs(void) {
    register unsigned int zero asm("$0");
    register volatile int *ptr asm("$8");
    register unsigned int a asm("$11");
    register unsigned int b asm("$12");
    register unsigned int count asm("$13");
    register volatile int *ptr1 asm("$9");

    ptr = (volatile int *)0x80070E0C;
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    a = zero | 1;
    ptr[0x10] = a;
    a = zero | 2;
    ptr[0xF] = a;
    asm volatile("" : : "r"(ptr), "r"(a));
    count = zero | 0xE;
    do {
        a = ptr[0x10];
        b = ptr[0xF];
        ptr--;
        a += b;
        ptr[0xF] = a;
    } while (count != 0 && (count--, 1));

    ptr1 = (volatile int *)0x80070E04;
    g_TaskGpuReg2 = (int *)0x80070E08;
    a = zero | 0x40;
    b = zero | 0x10;
    *ptr1 = a;
    *g_TaskGpuReg2 = b;
}
