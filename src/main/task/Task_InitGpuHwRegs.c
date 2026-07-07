/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --store-return-delay */

void Task_InitGpuHwRegs(void) {
    register volatile int *ptr asm("$8");
    register unsigned int a asm("$11");
    register unsigned int b asm("$12");
    register unsigned int count asm("$13");
    register volatile int *ptr1 asm("$9");
    register volatile int *ptr2 asm("$10");

    ptr = (volatile int *)0x80070E0C;
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    asm volatile("ori %0, $0, 0x1" : "=r"(a));
    ptr[0x10] = a;
    asm volatile("ori %0, $0, 0x2" : "=r"(a));
    ptr[0xF] = a;
    asm volatile("" : : "r"(ptr), "r"(a));
    asm volatile("ori %0, $0, 0xE" : "=r"(count));
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
    asm volatile("ori %0, $0, 0x40" : "=r"(a));
    asm volatile("ori %0, $0, 0x10" : "=r"(b));
    *ptr1 = a;
    *ptr2 = b;
}
