#include "common.h"
/* MASPSX_FLAGS: --stack-return-delay */

extern int (*D_8009B72C)(void *obj, int needs_ack);

int MemCard_WriteByte(void *obj, int value);

int MemCard_WriteByteWithAckCheck(void *obj) {
    register void *card asm("$16");
    void *call_obj;
    int needs_ack;
    int value;
    register int result asm("$3");
    int ret;
    card = obj;
    needs_ack = 0;
    if ((*(u8 *)*(void **)((u8 *)card + 0x3C) >> 4) == 8) {
        needs_ack = *(u8 *)((u8 *)card + 0x36) < 1;
    }

    asm volatile("" : : : "$4");
    call_obj = card;
    value = D_8009B72C(call_obj, needs_ack);
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
