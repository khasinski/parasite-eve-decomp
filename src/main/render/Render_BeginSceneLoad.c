typedef unsigned int u32;

extern u32 g_RenderStateFlags;

int Render_BeginSceneLoad(void) {
    u32 *flags;
    u32 temp;
    u32 old;
    u32 value;
    u32 next;
    register u32 mask asm("$4");
    register u32 *flags2 asm("$3");
    register u32 value2 asm("$2");

    flags = &g_RenderStateFlags;
    old = *flags;
    value = old | 0x1000;
    *flags = value;
    if (value & 0x2000) {
        temp = value & ~0x2000;
        next = temp;
    } else {
        next = old | 0x3000;
    }
    *flags = next;
    asm volatile("" : : : "memory");
    mask = 0xFFFF0000;
    flags2 = &g_RenderStateFlags;
    value2 = *flags2;
    mask |= 0x3FFF;
    value2 &= mask;
    value2 |= 0x4000;
    *flags2 = value2;
    return 0;
}
