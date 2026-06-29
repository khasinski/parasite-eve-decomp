extern char * volatile g_GeomState;
extern unsigned char g_GeomGroupSel;
extern unsigned short D_800BCFAC;
extern unsigned short g_RenderSavedBoundsMaxX;
extern unsigned short D_800BCFB0;
extern unsigned short g_RenderSavedBoundsMaxY;

int Render_SaveAndOpenBounds(void) {
    char *base = g_GeomState;
    char *entry = g_GeomState + *(int *)(base + 0x1C) + (g_GeomGroupSel * 52);

    D_800BCFAC = *(unsigned short *)(entry + 0x2C);
    g_RenderSavedBoundsMaxX = *(unsigned short *)(entry + 0x2E);
    D_800BCFB0 = *(unsigned short *)(entry + 0x30);
    g_RenderSavedBoundsMaxY = *(unsigned short *)(entry + 0x32);

    *(short *)(entry + 0x2C) = -0x8000;
    *(short *)(entry + 0x2E) = 0x7FFF;
    *(short *)(entry + 0x30) = -0x8000;
    *(short *)(entry + 0x32) = 0x7FFF;

    return 0;
}
