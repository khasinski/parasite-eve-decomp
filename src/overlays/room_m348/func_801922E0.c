typedef short s16;
typedef unsigned char u8;
typedef unsigned short u16;

void func_801922E0(int arg0, u8 *arg1, u8 *arg2) {
    register u8 *ctrl asm("t1") = arg1;
    register int i asm("t0") = 0;
    register u8 *timer asm("a3") = arg2;
    register u8 *obj asm("a2") = arg2;
    u16 value;

    __asm__ volatile("" : "=r"(ctrl), "=r"(i), "=r"(timer), "=r"(obj) : "0"(ctrl), "1"(i), "2"(timer), "3"(obj));
    do {
        *(u16 *)(obj + 0x30) += *(u16 *)(obj + 0x50);
        *(u16 *)(obj + 0x32) += *(u16 *)(obj + 0x52);
        *(u16 *)(obj + 0x34) += *(u16 *)(obj + 0x54);

        value = *(u16 *)(timer + 0x78) - 8;
        *(u16 *)(timer + 0x78) = value;
        if ((s16)value < 0) {
            *(u16 *)(timer + 0x78) = 0;
        }

        timer += 2;
        i++;
        obj += 8;
    } while ((unsigned int)i < 4);

    if (*(s16 *)(ctrl + 2) >= 0x3D) {
        ctrl[1] = 2;
    }
}
