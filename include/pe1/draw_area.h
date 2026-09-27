#ifndef PE1_DRAW_AREA_H
#define PE1_DRAW_AREA_H

#include "common.h"
#include "pe1/psyq_gpu.h"

typedef struct DrawAreaRect {
    s16 x, y;
    u16 w, h;
} DrawAreaRect;

void SetDrawArea(GpuCmdPacket *packet, DrawAreaRect *rect);

/* PSX DMA tags keep the word count in the high byte and a physical address
 * in the low 24 bits. The pointer conversion is part of that hardware ABI. */
static inline void Gpu_LinkDrawArea(u32 *entry, GpuCmdPacket *packet)
{
    packet->u0.tag = (packet->u0.tag & 0xFF000000) | (*entry & 0xFFFFFF);
    *entry = (*entry & 0xFF000000) | ((u32)packet & 0xFFFFFF);
}

#endif
