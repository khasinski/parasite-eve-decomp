/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -G8 -fno-schedule-insns */
/*
 * PsyQ 4.x VSync (retail asm: asm/USA/main/psyq/libapi/VSync.s).
 * gcc 2.8.1 (filled jr-delay-slot epilogue) with a small-data threshold so
 * every global access is an assembler macro (per-access lui, $at stores),
 * matching the retail codegen. Symbol names follow the asm unit:
 *   D_80094574  pointer variable holding the GPU status port address
 *   D_80094578  pointer variable holding the timer-1 counter port address
 *   D_8009457C  horizontal count base (debounced timer snapshot)
 *   D_80094580  vertical count base (previous VSync counter)
 *   g_VSyncCount  VSync interrupt counter
 *   v_wait  v_wait
 * The mode<0 return going through the `target` temp is load-bearing: it
 * gives the temp a $2 copy-preference so gcc's global allocator leaves
 * `mode` in $4 (retail allocation) instead of evicting it to $6.
 */
extern volatile int *D_80094574;
extern volatile int *D_80094578;
extern volatile int D_8009457C;
extern int D_80094580;
extern volatile int g_VSyncCount;

void v_wait(int arg0, int arg1);

int VSync(int mode) {
    int stat;
    int n;
    int ret;
    int target;
    int frames;
    volatile int count;

    stat = *D_80094574;
    do {
        count = *D_80094578;
    } while (count != *D_80094578);
    ret = (count - D_8009457C) & 0xFFFF;
    if (mode < 0) {
        target = g_VSyncCount;
        return target;
    }
    if (mode == 1) {
        return ret;
    }
    n = 1;
    if (mode > 0) {
        target = D_80094580 - n + mode;
    } else {
        target = D_80094580;
    }
    frames = mode > 0 ? mode - n : 0;
    v_wait(target, frames);
    stat = *D_80094574;
    v_wait(g_VSyncCount + 1, 1);
    if (stat & 0x400000) {
        if ((stat ^ *D_80094574) >= 0) {
            do {
            } while (!((stat ^ *D_80094574) & 0x80000000));
        }
    }
    D_80094580 = g_VSyncCount;
    do {
        D_8009457C = *D_80094578;
    } while (D_8009457C != *D_80094578);
    return ret;
}

/*
 * Alternative-entry symbol present in the retail object (spimdisasm alabel
 * inside the 0x80000000 wait loop). Emits no code; only mirrors the symbol
 * layout of the target object.
 */
__asm__(".globl g_GameThrottle # maspsx-keep\n"
        "\t.set g_GameThrottle, VSync+0x58 # maspsx-keep\n"
        "\t.type g_GameThrottle, @function # maspsx-keep\n"
        "\t.size g_GameThrottle, 0xB8 # maspsx-keep\n"
        "\t.globl func_80073B54 # maspsx-keep\n"
        "\t.set func_80073B54, VSync+0x110 # maspsx-keep\n"
        "\t.type func_80073B54, @function # maspsx-keep\n"
        "\t.size func_80073B54, 0x68 # maspsx-keep\n"
        "\t.size VSync, 0x58 # maspsx-keep\n");
