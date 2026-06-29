typedef unsigned char u8;

extern int g_RenderStateFlags;
extern u8 g_RenderFadeFrameCount;
extern u8 g_RenderFadeFrameCounter;

int Render_StartFadeIn(int arg0) {
    register int *flags asm("$2");
    int value;

    flags = &g_RenderStateFlags;
    g_RenderFadeFrameCount = arg0;
    g_RenderFadeFrameCounter = 0;
    value = *flags;
    value &= ~0xC00;
    value |= 0x400;
    *flags = value;
    return 0;
}
