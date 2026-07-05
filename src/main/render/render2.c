typedef unsigned char u8;
typedef unsigned short u16;

extern int D_8003E60C;

void Render_ApplyColourOffset(void *arg0, int arg1, int arg2, int arg3) {
    char unused[0x10];
    void *obj;
    int outer;
    int base;
    int inner;
    register char *ptr asm("$8");
    register int saved_z asm("$13");
    register int work7 asm("$7");
    register int z_signed asm("$4");
    register int tmp asm("$2");
    register int tmp2 asm("$3");

    obj = arg0;
    asm volatile("" : "=r"(obj) : "0"(obj));
    outer = 0;
    tmp = *(u16 *)(*(char **)obj + 8);
    base = *(int *)((char *)obj + 0x54);
    saved_z = arg3;
    if (tmp > 0) {
        work7 = arg3 << 24;
        do {
            inner = 0;
            ptr = (char *)base + 0xE;
            z_signed = work7 >> 24;
            do {
                inner++;
                base += 0x34;
                tmp = *(u8 *)(ptr - 1);
                tmp2 = *(u8 *)(ptr - 2);
                tmp += arg2;
                *(u8 *)(ptr - 1) = tmp;
                tmp = *(u8 *)(ptr + 0xB);
                tmp2 += arg1;
                *(u8 *)(ptr - 2) = tmp2;
                tmp2 = *(u8 *)(ptr + 0xA);
                tmp += arg2;
                *(u8 *)(ptr + 0xB) = tmp;
                tmp = *(u8 *)(ptr + 0x17);
                tmp2 += arg1;
                *(u8 *)(ptr + 0xA) = tmp2;
                tmp2 = *(u8 *)(ptr + 0x16);
                tmp += arg2;
                *(u8 *)(ptr + 0x17) = tmp;
                tmp = *(u8 *)(ptr + 0x23);
                tmp2 += arg1;
                *(u8 *)(ptr + 0x16) = tmp2;
                tmp2 = *(u16 *)ptr;
                tmp += arg2;
                tmp2 += z_signed;
                *(u8 *)(ptr + 0x23) = tmp;
                *(u16 *)ptr = tmp2;
                asm volatile("" : : : "memory");
                tmp = inner < 2;
                ptr += 0x34;
            } while (tmp != 0);

            tmp = *(u16 *)(*(char **)obj + 8);
            outer++;
        } while (outer < tmp);
    }

    tmp = *(int *)obj;
    asm volatile("addu %0, %1, $0" : "=r"(ptr) : "r"(base));
    tmp = *(u16 *)(tmp + 0xA);
    if (tmp > 0) {
        outer = 0;
        base = saved_z << 24;
        inner = 0;
        do {
            work7 = (int)(ptr + 0xE);
            z_signed = base >> 24;
            do {
                inner++;
                ptr += 0x28;
                tmp = *(u8 *)(work7 - 1);
                tmp2 = *(u8 *)(work7 + 0xB);
                tmp += arg2;
                *(u8 *)(work7 - 1) = tmp;
                tmp = *(u8 *)(work7 - 2);
                tmp2 += arg2;
                *(u8 *)(work7 + 0xB) = tmp2;
                tmp2 = *(u8 *)(work7 + 0x17);
                tmp += arg1;
                *(u8 *)(work7 - 2) = tmp;
                tmp = *(u8 *)(work7 + 0xA);
                tmp2 += arg2;
                *(u8 *)(work7 + 0x17) = tmp2;
                tmp2 = *(u16 *)work7;
                tmp += arg1;
                *(u8 *)(work7 + 0xA) = tmp;
                tmp = *(u8 *)(work7 + 0x16);
                tmp2 += z_signed;
                *(u16 *)work7 = tmp2;
                tmp += arg1;
                *(u8 *)(work7 + 0x16) = tmp;
                tmp = inner < 2;
                work7 += 0x28;
            } while (tmp != 0);

            tmp = *(int *)obj;
            tmp = *(u16 *)(tmp + 0xA);
            asm volatile("" : "=r"(tmp) : "0"(tmp));
            outer++;
            inner = 0;
        } while (outer < tmp);
    }
}

void Render_IncrementCounter(void) {
    register int *ptr asm("$4");
    register int value asm("$5");

    ptr = &D_8003E60C;
    value = *ptr;
    value++;
    *ptr = value;
}
