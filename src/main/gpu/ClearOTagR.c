/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;
typedef int s32;

#include "../../../tools/m2c/m2c_macros.h"

extern u8 D_8009574E[];
extern s32 (*D_80095748[])(char *, void *, s32);
extern void *D_80095744[];
extern char D_80011910[];
extern s32 D_800957F8[];
extern s32 D_8009580C[];

void ClearOTagR(void *packet, s32 count) {
    register s32 (*debug_cb)(char *, void *, s32) asm("$2");
    register void (*clear_cb)(void *, s32) asm("$2");
    register s32 mask24 asm("$6");
    register s32 temp_a0 asm("$4");

    if (D_8009574E[0] >= 2U) {
        register char *fmt asm("$4");
        register void *debug_packet asm("$5");
        register s32 debug_count asm("$6");

        fmt = D_80011910;
        asm volatile("" : "=r"(fmt) : "0"(fmt));
        debug_packet = packet;
        debug_count = count;
        debug_cb = D_80095748[0];
        debug_cb(fmt, debug_packet, debug_count);
    }

    clear_cb = *(void (**)(void *, s32))((u8 *)D_80095744[0] + 0x2C);
    clear_cb(packet, count);

    mask24 = 0xFFFFFF;
    {
        register s32 *tail asm("$2");
        register s32 *tag_ptr asm("$5");
        register s32 tag_value asm("$3");

        tail = packet;
        asm volatile("" : "=r"(tail) : "0"(tail));
        tag_ptr = D_8009580C;
        tag_value = (s32)D_800957F8;
        tag_value &= mask24;
        temp_a0 = 0x04000000;
        tag_value |= temp_a0;
        *tag_ptr = tag_value;
        tag_ptr = (s32 *)((s32)tag_ptr & mask24);
        *tail = (s32)tag_ptr;
    }
}
