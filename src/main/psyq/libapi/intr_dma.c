#include "pe1/psyq_api_internal.h"

DmaInterruptCallback setIntrDMA(int channel, DmaInterruptCallback callback) {
    register int channelReg asm("$6");
    register DmaInterruptCallback callbackReg asm("$4");
    register DmaInterruptCallback *base;
    DmaInterruptCallback *slot;
    DmaInterruptCallback old;
    register DmaInterruptCallback ret asm("$2");
    volatile u32 *dmaReg;
    u32 mask;
    u32 tmp;
    channelReg = channel;
    asm volatile("" : : "r"(channelReg));
    callbackReg = callback;
    base = g_IntrDmaHandlerTable;
    slot = &base[channelReg];
    old = *slot;
    ret = old;

    if (callbackReg != old) {
        if (callbackReg != 0) {
            register int bitSet asm("$3");
            register u32 maskSet asm("$4");

            dmaReg = g_IntrDmaDispatchPtr;
            tmp = 0xFFFFFF;
            *slot = callbackReg;
            maskSet = *dmaReg;
            bitSet = channelReg + 0x10;
            maskSet &= tmp;
            tmp = 1;
            tmp <<= bitSet;
            bitSet = 0x800000;
            tmp |= bitSet;
            maskSet |= tmp;
            *dmaReg = maskSet;
            asm volatile("" : : : "memory");
            ret = old;
            return ret;
        } else {
            register int bitClear asm("$4");
            register u32 maskClear asm("$3");
            register DmaInterruptCallback zeroReg asm("$0");

            dmaReg = g_IntrDmaDispatchPtr;
            tmp = 0xFFFFFF;
            *slot = zeroReg;
            maskClear = *dmaReg;
            bitClear = channelReg + 0x10;
            maskClear &= tmp;
            tmp = 0x800000;
            maskClear |= tmp;
            tmp = 1;
            tmp <<= bitClear;
            tmp = ~tmp;
            maskClear &= tmp;
            *dmaReg = maskClear;
            asm volatile("" : : : "memory");
            ret = old;
            return ret;
        }
    }

    return ret;
}

void memclrIntrDMA(int *ptr, int count) {
    int i = count - 1;

    if (count != 0) {
        do {
            *ptr = 0;
            i--;
            ptr++;
        } while (i != -1);
    }
}
