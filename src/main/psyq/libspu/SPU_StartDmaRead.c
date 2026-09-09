/* SPDX-License-Identifier: MIT */
/* Adapted from sozud/psy-q-decomp src/spu/s_crwa.c; see THIRD_PARTY_NOTICES.md. */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"
#include "pe1/psyq_spu_internal.h"

extern s32 g_SpuReverbWorkAreaTable[];
extern s32 _spu_mem_mode_plus;
extern s32 D_8009B418;
extern s32 D_8009B384;
extern s32 D_8009C4C0[];
int _SpuIsInAllocateArea_(u32);
int _spu_t(int, ...);
int WaitEvent(int);

int SPU_StartDmaRead(int mode)
{
    SpuCallback volatile callback;
    s32 oldTransmode;
    s32 address;
    s32 more;
    s32 transmodeCleared;
    u32 size;
    u32 length;

    callback = 0;
    transmodeCleared = 0;
    if (mode >= 0xAU) {
        return -1;
    }
    {
        u32 offset;
        s32 *base = g_SpuReverbWorkAreaTable;
        offset = mode << 2;
        /* Reuse the address temporary for the remaining byte count below. */
        length = offset + (u32)base;
    }
    if (_SpuIsInAllocateArea_(*(s32 *)length) != 0) {
        return -1;
    }
    if (mode == 0) {
        length = 0x10 << _spu_mem_mode_plus;
        address = 0xFFF0 << _spu_mem_mode_plus;
    } else {
        s32 start = *(s32 *)length;
        length = (0x10000 - start) << _spu_mem_mode_plus;
        address = start << _spu_mem_mode_plus;
    }
    oldTransmode = D_8009B418;
    if (D_8009B418 == 1) {
        D_8009B418 = 0;
        /* Materialize the restore flag independently of the comparison value. */
        asm volatile("" ::: "$2");
        transmodeCleared = 1;
    }
    more = 1;
    if (_spu_transferCallback != 0) {
        callback = _spu_transferCallback;
        _spu_transferCallback = 0;
    }
    do {
        size = 0x400;
        if (length <= 0x400U) {
            size = length;
            more = 0;
        }
        _spu_t(2, address);
        _spu_t(1);
        _spu_t(3, &D_8009C4C0, size);
        WaitEvent(D_8009B384);
        length -= 0x400;
        address += 0x400;
    } while (more != 0);
    if (transmodeCleared != 0) {
        D_8009B418 = oldTransmode;
    }
    if (callback != 0) {
        _spu_transferCallback = callback;
    }
    return 0;
}
