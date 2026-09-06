/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern volatile u16 *_spu_RXX;
extern volatile u32 *g_SpuDmaMadrPtr;
extern volatile u32 *g_SpuDmaBcrPtr;
extern volatile u32 *g_SpuDmaChcrPtr;
extern s32 g_SpuDmaDirection;
void _spu_Fw1ts(void);
void _spu_FsetDelayR(void);

void _spu_Fr_(void *address, u16 spuAddress, u32 blocks)
{
    _spu_RXX[0xD3] = spuAddress;
    _spu_Fw1ts();
    _spu_RXX[0xD5] |= 0x30;
    _spu_Fw1ts();
    _spu_FsetDelayR();
    *g_SpuDmaMadrPtr = (u32)address;
    *g_SpuDmaBcrPtr = (blocks << 16) | 0x10;
    g_SpuDmaDirection = 1;
    *g_SpuDmaChcrPtr = 0x1000200;
}
