#include "common.h"
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_spu_internal.h"
extern u16 g_SpuTransferAddr;
extern volatile u32 D_8009B450;
extern u32 D_8009B454;
extern s32 D_8009B418;

void _spu_Fr_(void *address, u16 spuAddress, u32 blocks)
{
    _spu_RXX->trans_addr = spuAddress;
    _spu_Fw1ts();
    _spu_RXX->spucnt |= 0x30;
    _spu_Fw1ts();
    _spu_FsetDelayR();
    *g_SpuDmaMadrPtr = (u32)address;
    *g_SpuDmaBcrPtr = (blocks << 16) | 0x10;
    g_SpuDmaDirection = 1;
    *g_SpuDmaChcrPtr = 0x1000200;
}

int _spu_t(int command, ...)
{
    /* PSX varargs ABI: the compiler spills a0-a3 into the argument area. */
    char *args = (char *)(&command + 1);
    u32 count;
    u32 size;
    u32 control;
    u16 mode;

    switch (command) {
    case 2:
        size = *(u32 *)((args += 4) - 4);
        g_SpuTransferAddr = size >> _spu_mem_mode_plus;
        _spu_RXX->trans_addr = g_SpuTransferAddr;
        break;
    case 1:
        g_SpuDmaDirection = 0;
        count = 0;
        while (_spu_RXX->trans_addr != g_SpuTransferAddr) {
            if (++count > 0xF00) {
                return -2;
            }
        }
        _spu_RXX->spucnt = (_spu_RXX->spucnt & ~0x30) | 0x20;
        break;
    case 0:
        g_SpuDmaDirection = 1;
        count = 0;
        while (_spu_RXX->trans_addr != g_SpuTransferAddr) {
            if (++count > 0xF00) {
                return -2;
            }
        }
        _spu_RXX->spucnt |= 0x30;
        break;
    case 3:
        mode = g_SpuDmaDirection == 1 ? 0x30 : 0x20;
        count = 0;
        while ((_spu_RXX->spucnt & 0x30) != mode) {
            if (++count > 0xF00) {
                return -2;
            }
        }
        if (g_SpuDmaDirection == 1) {
            args += 4;
            _spu_FsetDelayR();
            control = 0x1000000;
        } else {
            args += 4;
            _spu_FsetDelayW();
            control = 0x1000000;
        }
        size = *(u32 *)(args - 4);
        D_8009B450 = size;
        size = *(u32 *)args;
        D_8009B454 = (size >> 6) + ((size & 0x3F) != 0);
        *g_SpuDmaMadrPtr = D_8009B450;
        *g_SpuDmaBcrPtr = (D_8009B454 << 16) | 0x10;
        if (g_SpuDmaDirection != 1) {
            control |= 0x201;
        } else {
            control = 0x1000200;
        }
        *g_SpuDmaChcrPtr = control;
        break;
    }
    return 0;
}

u32 _spu_Fw(void *address, u32 size)
{
    if (D_8009B418 == 0) {
        _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
        _spu_t(1);
        _spu_t(3, address, size);
    } else {
        _spu_FwriteByIO(address, size);
    }
    return size;
}

s32 _spu_Fr(void *address, s32 size) {
    _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
    _spu_t(0);
    _spu_t(3, address, size);
    return size;
}
