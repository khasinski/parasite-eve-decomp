/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"

extern short D_80095750;
extern short D_80095752;
extern unsigned int D_800A3300[];
extern unsigned int D_800A3328[];
extern unsigned int *D_80095854;
unsigned int _param(int parameter);
void Gpu_StartDmaTransfer(unsigned int packet);

int Gpu_DmaVramTransfer(RECT *rect, unsigned int color) {
    unsigned short width, height;
    if (rect->w >= 0) {
        if (D_80095750 - 1 < rect->w) width = D_80095750 - 1;
        else width = rect->w;
    } else width = 0;
    rect->w = width;
    if (rect->h >= 0) {
        if (D_80095752 - 1 < rect->h) height = D_80095752 - 1;
        else height = rect->h;
    } else height = 0;
    rect->h = height;
    if ((rect->x & 63) || (rect->w & 63)) {
        D_800A3300[0] = ((unsigned int)D_800A3328 & 0xFFFFFF) | 0x08000000;
        D_800A3300[1] = 0xE3000000;
        D_800A3300[2] = 0xE4FFFFFF;
        D_800A3300[3] = 0xE5000000;
        D_800A3300[4] = 0xE6000000;
        D_800A3300[5] = 0xE1000000 | (*D_80095854 & 0x7FF) | ((color >> 31) << 10);
        D_800A3300[6] = (color & 0xFFFFFF) | 0x60000000;
        D_800A3300[7] = *(unsigned int *)&rect->x;
        D_800A3300[8] = *(unsigned int *)&rect->w;
        D_800A3328[0] = 0x03FFFFFF;
        D_800A3328[1] = _param(3) | 0xE3000000;
        D_800A3328[2] = _param(4) | 0xE4000000;
        D_800A3328[3] = _param(5) | 0xE5000000;
    } else {
        D_800A3300[0] = 0x05FFFFFF;
        D_800A3300[1] = 0xE6000000;
        D_800A3300[2] = 0xE1000000 | (*D_80095854 & 0x7FF) | ((color >> 31) << 10);
        D_800A3300[3] = (color & 0xFFFFFF) | 0x02000000;
        D_800A3300[4] = *(unsigned int *)&rect->x;
        D_800A3300[5] = *(unsigned int *)&rect->w;
    }
    {
        /* Fixed USA packet address; keep page and offset separate. */
        unsigned int packet = 0x800A0000;
        asm volatile("" : "+r"(packet));
        Gpu_StartDmaTransfer(packet + 0x3300);
    }
    {
        int result = 0;
        asm volatile("" : "+r"(result));
        return result;
    }
}
