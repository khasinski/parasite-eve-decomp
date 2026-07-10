/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;

extern int (*D_8009B72C)(void *obj, int needs_ack);

int MemCard_WriteByte(void *obj, int value);

int MemCard_WriteByteWithAckCheck(void *obj) {
    register void *card asm("$16");
    register void *call_obj asm("$4");
    int needs_ack;
    int value;
    register int result asm("$3");
    register int ret asm("$2");

    card = obj;
    needs_ack = 0;
    if ((*(u8 *)*(void **)((u8 *)card + 0x3C) >> 4) == 8) {
        needs_ack = *(u8 *)((u8 *)card + 0x36) < 1;
    }

    asm volatile("" : : : "$4");
    call_obj = card;
    value = D_8009B72C(call_obj, needs_ack);
    asm volatile("" : : : "$4");
    call_obj = card;
    result = MemCard_WriteByte(call_obj, value & 0xFF);
    asm volatile(
        ".word 0x2402005A\n"
        ".word 0x10620006\n"
        ".word 0x00601021\n"
        ".word 0x10600004\n"
        ".word 0x00000000\n"
        ".word 0x04610002\n"
        ".word 0x2402FFF7\n"
        ".word 0x00601021\n"
        : "=r"(ret)
        : "r"(result));
    return ret;
}
