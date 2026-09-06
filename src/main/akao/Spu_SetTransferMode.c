/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

typedef void (*SpuCallback)(void);

extern SpuCallback D_8009B438;
extern void _SpuCallback(SpuCallback callback);

/* Historical name: this installs a callback, not a transfer mode. */
SpuCallback Spu_SetTransferMode(SpuCallback callback)
{
    SpuCallback previous = D_8009B438;

    if (callback != previous) {
        /* 0x800A0000 - 0x4BC8 is D_8009B438. Keep the store address
         * separate so its single instruction can fill the call delay slot. */
        register SpuCallback *page asm("$1") = (SpuCallback *)0x800A0000;
        __asm__("" : "+r"(page));
        page[-0x12F2] = callback;
        _SpuCallback(callback);
    }
    return previous;
}
