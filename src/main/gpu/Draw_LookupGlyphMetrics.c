typedef unsigned char u8;

extern u8 g_GlyphMetricsTable[];
extern int D_800A8050;

int Draw_LookupGlyphMetrics(int arg0) {
    arg0 += D_800A8050;
    return g_GlyphMetricsTable[arg0];
}
