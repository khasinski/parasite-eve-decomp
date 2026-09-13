/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"
#include "pe1/psyq_spu_internal.h"
#include "pe1/akao/spu_common.h"

extern u32 g_SpuReverbWorkAreaTable[];
extern SpuReverbRegisterAttrs g_SpuReverbPresetRegisters[];
extern int _SpuIsInAllocateArea_(u32);
extern int SpuClearReverbWorkArea(int);

int SPU_StepDmaRead(u32 mode) {
    SpuReverbRegisterAttrs attr;
    int clear = 0;
    u32 enabled;
    u32 *area;
    unsigned char *src;
    unsigned char *dst;
    int i;
    int occupied;
    if (mode & 0x100) {
        mode &= ~0x100;
        clear = 1;
    }
    if (mode >= 10) return -1;
    {
        register u32 offset asm("$2") = mode << 2;
        area = (u32 *)((unsigned char *)g_SpuReverbWorkAreaTable + offset);
    }
    occupied = _SpuIsInAllocateArea_(*area);
    dst = (unsigned char *)&attr;
    if (occupied) return -1;
    D_8009B3A0.mode = mode;
    _spu_rev_offsetaddr = *area;
    src = (unsigned char *)&g_SpuReverbPresetRegisters[mode];
    for (i = sizeof(attr) - 1; i != -1; i--) {
        unsigned char byte = *src++;
        *dst++ = byte;
    }
    attr.mask = 0;
    switch (mode) {
    case 7:
        D_8009B3A0.feedback = 0x7F;
        D_8009B3A0.delay = 0x7F;
        break;
    case 8:
        D_8009B3A0.feedback = 0;
        D_8009B3A0.delay = 0x7F;
        break;
    default:
        D_8009B3A0.feedback = 0;
        D_8009B3A0.delay = 0;
        break;
    }
    {
        u32 current = (_spu_RXX->spucnt >> 7) & 1;
        enabled = current;
        if (current) _spu_RXX->spucnt &= ~0x80;
    }
    _spu_RXX->reverb_volume_left = 0;
    _spu_RXX->reverb_volume_right = 0;
    D_8009B3A0.depth[0] = 0;
    D_8009B3A0.depth[1] = 0;
    _spu_setReverbAttr(&attr);
    if (clear) SpuClearReverbWorkArea(mode);
    _spu_FsetRXX(0xD1, _spu_rev_offsetaddr, 0);
    if (enabled) _spu_RXX->spucnt |= 0x80;
    return 0;
}
