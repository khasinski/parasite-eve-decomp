/* MASPSX_FLAGS: --store-return-delay */

void Task_InitGpuHwRegs(void) {
    register unsigned int zero asm("$0");
    register volatile int *ptr asm("$8");
    register unsigned int a asm("$11");
    register unsigned int b asm("$12");
    register unsigned int count asm("$13");
    register volatile int *ptr1 asm("$9");
    register volatile int *ptr2 asm("$10");

    ptr = (volatile int *)0x80070E0C;
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    a = zero | 1;
    ptr[0x10] = a;
    a = zero | 2;
    ptr[0xF] = a;
    asm volatile("" : : "r"(ptr), "r"(a));
    count = zero | 0xE;
loop:
    a = ptr[0x10];
    b = ptr[0xF];
    ptr--;
    a += b;
    ptr[0xF] = a;
    if (count != 0) {
        count--;
        goto loop;
    }

    ptr1 = (volatile int *)0x80070E04;
    ptr2 = (volatile int *)0x80070E08;
    a = zero | 0x40;
    b = zero | 0x10;
    *ptr1 = a;
    *ptr2 = b;
}
