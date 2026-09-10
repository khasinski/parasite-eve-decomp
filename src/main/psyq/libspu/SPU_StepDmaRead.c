/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"
#include "pe1/psyq_spu_internal.h"
#include "pe1/akao/spu_common.h"

extern u32 g_SpuReverbWorkAreaTable[];
extern SpuReverbRegisterAttrs g_SpuReverbPresetRegisters[];
extern u32 g_SpuReverbMode, _spu_rev_offsetaddr;
extern int g_SpuReverbDelayFeedback[2];
extern short g_SpuReverbDepth[2];
extern int _SpuIsInAllocateArea_(u32);
extern int SPU_StartDmaRead(int);

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
    g_SpuReverbMode = mode;
    _spu_rev_offsetaddr = *area;
    src = (unsigned char *)&g_SpuReverbPresetRegisters[mode];
    for (i = sizeof(attr) - 1; i != -1; i--) {
        unsigned char byte = *src++;
        *dst++ = byte;
    }
    attr.mask = 0;
    switch (mode) {
    case 7:
        g_SpuReverbDelayFeedback[1] = 0x7F;
        g_SpuReverbDelayFeedback[0] = 0x7F;
        break;
    case 8:
        g_SpuReverbDelayFeedback[1] = 0;
        g_SpuReverbDelayFeedback[0] = 0x7F;
        break;
    default:
        g_SpuReverbDelayFeedback[1] = 0;
        g_SpuReverbDelayFeedback[0] = 0;
        break;
    }
    {
        u32 current = (_spu_RXX->spucnt >> 7) & 1;
        enabled = current;
        if (current) _spu_RXX->spucnt &= ~0x80;
    }
    _spu_RXX->reverb_volume_left = 0;
    _spu_RXX->reverb_volume_right = 0;
    g_SpuReverbDepth[0] = 0;
    g_SpuReverbDepth[1] = 0;
    _spu_setReverbAttr(&attr);
    if (clear) SPU_StartDmaRead(mode);
    _spu_FsetRXX(0xD1, _spu_rev_offsetaddr, 0);
    if (enabled) _spu_RXX->spucnt |= 0x80;
    return 0;
}
