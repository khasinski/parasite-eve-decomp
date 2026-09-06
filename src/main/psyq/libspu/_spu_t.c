/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern volatile u16 *_spu_RXX;
extern u16 g_SpuTransferAddr;
extern s32 _spu_mem_mode_plus;
extern s32 g_SpuDmaDirection;
extern volatile u32 D_8009B450;
extern u32 D_8009B454;
extern volatile u32 *g_SpuDmaMadrPtr;
extern volatile u32 *g_SpuDmaBcrPtr;
extern volatile u32 *g_SpuDmaChcrPtr;
void _spu_FsetDelayR(void);
void _spu_FsetDelayW(void);

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
        _spu_RXX[0xD3] = g_SpuTransferAddr;
        break;
    case 1:
        g_SpuDmaDirection = 0;
        count = 0;
        while (_spu_RXX[0xD3] != g_SpuTransferAddr) {
            if (++count > 0xF00) {
                return -2;
            }
        }
        _spu_RXX[0xD5] = (_spu_RXX[0xD5] & ~0x30) | 0x20;
        break;
    case 0:
        g_SpuDmaDirection = 1;
        count = 0;
        while (_spu_RXX[0xD3] != g_SpuTransferAddr) {
            if (++count > 0xF00) {
                return -2;
            }
        }
        _spu_RXX[0xD5] |= 0x30;
        break;
    case 3:
        mode = g_SpuDmaDirection == 1 ? 0x30 : 0x20;
        count = 0;
        while ((_spu_RXX[0xD5] & 0x30) != mode) {
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
        /* Keep CHCR low-bit preparation after the DMA register writes. */
        asm volatile("" : "+r"(control));
        control |= 0x201;
        if (g_SpuDmaDirection == 1) {
            control = 0x1000200;
        }
        *g_SpuDmaChcrPtr = control;
        break;
    }
    return 0;
}
