typedef unsigned int u32;

extern void *D_800956C0[];
extern u32 *D_800956BC;

void *setIntrDMA(int channel, void *callback) {
    register int channelReg asm("$6");
    register void *callbackReg asm("$4");
    register void **base asm("$3");
    register int index asm("$2");
    register void **slot asm("$3");
    register void *old asm("$7");
    register void *ret asm("$2");
    register u32 *dmaReg asm("$5");
    register u32 mask asm("$4");
    register int bit asm("$3");
    register u32 tmp asm("$2");

    channelReg = channel;
    asm volatile("" : : "r"(channelReg));
    callbackReg = callback;
    base = D_800956C0;
    index = channelReg << 2;
    slot = (void **)((char *)base + index);
    old = *slot;
    ret = old;

    if (callbackReg != old) {
        if (callbackReg != 0) {
            register int bitSet asm("$3");
            register u32 maskSet asm("$4");

            dmaReg = D_800956BC;
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
            register void *zeroReg asm("$0");

            dmaReg = D_800956BC;
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
