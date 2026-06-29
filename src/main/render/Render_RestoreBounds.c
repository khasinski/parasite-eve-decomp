extern char * volatile g_GeomState;
extern unsigned char g_GeomGroupSel;
extern unsigned short D_800BCFAC;
extern unsigned short g_RenderSavedBoundsMaxX;
extern unsigned short D_800BCFB0;
extern unsigned short g_RenderSavedBoundsMaxY;

int Render_RestoreBounds(void) {
    char *base = g_GeomState;
    char *entry = g_GeomState + *(int *)(base + 0x1C) + (g_GeomGroupSel * 52);

    *(unsigned short *)(entry + 0x2C) = D_800BCFAC;
    *(unsigned short *)(entry + 0x2E) = g_RenderSavedBoundsMaxX;
    *(unsigned short *)(entry + 0x30) = D_800BCFB0;
    *(unsigned short *)(entry + 0x32) = g_RenderSavedBoundsMaxY;

    return 0;
}
