#include "common.h"
extern volatile u32 *gpu_status __asm__("D_80094574");
extern volatile u32 *hblank_counter __asm__("D_80094578");
extern volatile u32 last_hblank __asm__("D_8009457C");
extern int last_vblank __asm__("D_80094580");
extern volatile int g_VSyncCount;
void v_wait(int target, int frames);

int VSync(int mode) {
    u32 status;
    int elapsed;
    int target;
    int frames;
    volatile u32 sample;

    status = *gpu_status;
    do {
        sample = *hblank_counter;
    } while (sample != *hblank_counter);
    elapsed = (sample - last_hblank) & 0xFFFF;
    if (mode < 0) return g_VSyncCount;
    if (mode == 1) return elapsed;
    if (mode > 0) target = (last_vblank - 1) + mode;
    else target = last_vblank;
    frames = 0;
    if (mode > 0) frames = mode - 1;
    v_wait(target, frames);
    status = *gpu_status;
    v_wait(g_VSyncCount + 1, 1);
    if (status & 0x400000) {
        while (!((status ^ *gpu_status) & 0x80000000)) {}
    }
    last_vblank = g_VSyncCount;
    do {
        last_hblank = *hblank_counter;
    } while (last_hblank != *hblank_counter);
    return elapsed;
}
