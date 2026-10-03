/* GCC_VERSION: 2.8.1 */
#include "common.h"

extern int (*D_8009B72C)(void *obj, int needs_ack);

int MemCard_WriteByte(void *obj, int value);

int MemCard_WriteByteWithAckCheck(void *obj) {
    register void *card asm("$16");
    void *call_obj;
    int needs_ack;
    int value;
    register int result asm("$3");
    int ret;
    register u32 zero asm("$0");
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
    /* Match debt: architectural zero keeps the result copy in the first
     * branch delay slot; the barriers preserve the shared return path. */
    asm volatile("" : "=r"(zero));
    ret = 90;
    asm volatile("" : "=r"(ret) : "0"(ret));
    if (result == ret) {
        ret = result + zero;
        goto done;
    }
    ret = result + zero;
    asm volatile("" : "=r"(result), "=r"(ret) : "0"(result), "1"(ret));
    if (result == 0)
        goto done;
    /* Retail returns -9 for positive values other than 90, and preserves
     * negative values. The BGEZ delay slot assigns -9 before branching. */
    ret = -9;
    if (result >= 0)
        goto done;
    ret = result;
done:
    return ret;
}
