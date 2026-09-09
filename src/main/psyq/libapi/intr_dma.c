/* CC1_FLAGS: -fno-cse-follow-jumps */
#include "pe1/psyq_api_internal.h"

DmaInterruptCallback setIntrDMA(int channel, DmaInterruptCallback callback) {
    register DmaInterruptCallback callbackReg asm("$4");
    register DmaInterruptCallback *base;
    DmaInterruptCallback *slot;
    DmaInterruptCallback old;
    register DmaInterruptCallback ret asm("$2");
    volatile u32 *dmaReg;
    u32 tmp;
    callbackReg = callback;
    base = g_IntrDmaHandlerTable;
    slot = &base[channel];
    old = *slot;
    ret = old;

    if (callbackReg != old) {
        if (callbackReg != 0) {
            int bitSet;
            register u32 maskSet asm("$4");

            dmaReg = g_IntrDmaDispatchPtr;
            tmp = 0xFFFFFF;
            *slot = callbackReg;
            maskSet = *dmaReg;
            bitSet = channel + 0x10;
            maskSet &= tmp;
            tmp = 1;
            tmp <<= bitSet;
            bitSet = 0x800000;
            tmp |= bitSet;
            maskSet |= tmp;
            *dmaReg = maskSet;
            asm volatile("" : : "m"(*dmaReg));
            ret = old;
            return ret;
        } else {
            register u32 maskClear asm("$3");

            dmaReg = g_IntrDmaDispatchPtr;
            tmp = 0xFFFFFF;
            *slot = 0;
            maskClear = *dmaReg;
            maskClear &= tmp;
            tmp = 0x800000;
            maskClear |= tmp;
            tmp = ~(1U << (channel + 0x10));
            maskClear &= tmp;
            *dmaReg = maskClear;
            asm volatile("" : : "m"(*dmaReg));
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
