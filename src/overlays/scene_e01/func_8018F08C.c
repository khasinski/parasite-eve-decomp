typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_8009D20C;

int func_8018F08C(u8 *arg0, int arg1, int arg2, u8 *arg3, int arg4) {
    register u8 *base asm("t0") = arg0 + 0xC;
    register int arg1_reg asm("a1") = arg1;
    u8 *node;
    u8 *cur;

    if (arg2 == 0x19) {
        goto cmd19;
    }

    if (arg2 == 0x200) {
        goto cmd200;
    }

    node = (u8 *)0x200;
    goto loop_test;

cmd19:
    node = (u8 *)1;
    if (arg1_reg != (int)node) {
        goto ret;
    }
    *(u8 **)(arg0 + 0x10) = arg3;
    *(int *)arg3 = arg1_reg;
    goto loop_test;

cmd200:
    {
        node = D_8009D20C;
        *(u8 **)(base + 0x10) = node;
        if (node == 0) {
            goto ret;
        }
    }

loop:
    cur = *(u8 **)(base + 0x10);
    if (cur[0xC] != (u32)arg3) {
        goto advance;
    }
    if (cur[0xD] != arg4) {
        goto advance;
    }
    if ((*(u32 *)(cur + 0x98) & 0x10) == 0) {
        goto ret;
    }
advance:
    node = *(u8 **)(*(u8 **)(base + 0x10) + 4);
    __asm__ volatile("nop");

loop_test:
    *(u8 **)(base + 0x10) = node;
    if (node != 0) {
        goto loop;
    }

ret:
    return 0;
}
