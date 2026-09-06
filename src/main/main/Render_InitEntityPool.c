/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/gpu_callbacks.h"
typedef struct GpuState {
    volatile unsigned char variant;
    unsigned char queue;
    unsigned char debug;
    unsigned char padding;
    unsigned short width, height;
    int pending;
    void (*done)(void);
    unsigned char drawCache[0x5C];
    unsigned char displayCache[0x14];
} GpuState;
extern GpuState D_8009574C;
extern unsigned char D_8009574E;
extern unsigned char D_80095704[];
extern char D_800117E0[], D_80011800[];
extern unsigned short D_800957CC[][2], D_800957D8[][2];
extern GpuCallbacks *D_80095744;
extern void (*D_80095748)(char *, int);
int printf(char *, ...);
void GPU_memset(void *, int, int);
void ResetCallback(void);
void GPU_cw(unsigned int);
int Gpu_InitDmaQueue(int);

int Render_InitEntityPool(int mode) {
    GpuState *state;
    register void *drawCache asm("$4");
    switch (mode & 7) {
    case 0:
    case 3:
        {
            char *format;
            unsigned char *version;
            unsigned char *statePage;
            asm volatile("" ::: "$6");
            format = D_800117E0;
            version = D_80095704;
            /* Fixed USA state address used only by the diagnostic call. */
            statePage = (unsigned char *)0x80090000;
            asm volatile("" : "+r"(statePage) : "r"(format), "r"(version));
            printf(format, version, statePage + 0x574C);
        }
    case 5:
        state = &D_8009574C;
        GPU_memset(state, 0, 0x80);
        ResetCallback();
        GPU_cw((unsigned int)D_80095744 & 0xFFFFFF);
        state->variant = Gpu_InitDmaQueue(mode);
        drawCache = state->drawCache;
        state->queue = 1;
        {
            register unsigned int offset asm("$2") = state->variant * 4;
            register unsigned char *page asm("$3");
            asm volatile("" : : "r"(offset), "r"(drawCache) : "$3");
            page = (unsigned char *)0x80090000;
            asm volatile("" : "+r"(page) : "r"(offset), "r"(drawCache));
            page += offset;
            /* USA width table, four-byte entries; preserve read order. */
            state->width = *(volatile unsigned short *)(page + 0x57CC);
        }
        state->height = D_800957D8[state->variant][0];
        GPU_memset(drawCache, -1, 0x5C);
        GPU_memset(state->displayCache, -1, 0x14);
        return state->variant;
    default:
        if (D_8009574E >= 2) D_80095748(D_80011800, mode);
        return D_80095744->reset(1);
    }
}
