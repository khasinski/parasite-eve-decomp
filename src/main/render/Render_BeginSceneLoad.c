typedef unsigned int u32;

extern u32 g_RenderStateFlags;

int Render_BeginSceneLoad(void) {
    u32 *flags;
    u32 temp;
    u32 old;
    u32 value;
    u32 next;

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
    __asm__ volatile(
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "lui $a0, 0xFFFF\n"
        "lui $v1, %hi(g_RenderStateFlags)\n"
        "addiu $v1, $v1, %lo(g_RenderStateFlags)\n"
        "lw $v0, 0x0($v1)\n"
        "ori $a0, $a0, 0x3FFF\n"
        "and $v0, $v0, $a0\n"
        "ori $v0, $v0, 0x4000\n"
        "sw $v0, 0x0($v1)\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n");
    return 0;
}
