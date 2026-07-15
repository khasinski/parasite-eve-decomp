typedef unsigned char u8;
typedef unsigned short u16;

extern int D_8009CED8;
extern int D_8009CEDC;
extern int D_8009CEE0;
extern int D_8009CEE4;
extern int D_8009CEE8;
extern int D_8009CEEC;
extern u8 D_800A1878[];
extern u16 *D_800B0E50;
extern u16 *D_800B0E54;
extern u8 D_800BCDE1;
extern u8 D_800BCDE2;
extern u8 D_800BCDE3;
extern u8 D_800BCE3D;
extern u8 D_800BCE3E;
extern u8 D_800BCE3F;

void Menu_SaveBgApplyFadeStep(void) {
    int pos;
    int level;
    int scale_a;
    int scale_b;
    u16 *src;
    u16 *dst;
    int count;
    int i;

    pos = D_8009CEE8 + D_8009CEE4;
    D_8009CEE8 = pos;
    if (pos < 0) {
        D_8009CEE8 = 0;
        D_8009CEE4 = 0;
        D_8009CED8 = 0;
    } else if (pos >= D_8009CEE0) {
        D_8009CEE8 = D_8009CEE0 - 1;
        D_8009CEE4 = 0;
        D_8009CED8 = 7;
    }

    level = D_800A1878[D_8009CEE8];
    scale_a = (level * D_8009CEEC) << 5;
    scale_b = level * (D_8009CEEC + 0x100);
    src = D_800B0E50;
    dst = D_800B0E54;
    count = D_8009CEDC << 8;

    for (i = 0; i < count; i++) {
        u16 color = *src++;

        if (color != 0) {
            int r = color & 0x1F;
            int g = (color >> 5) & 0x1F;
            int b = (color >> 10) & 0x1F;
            int alpha = color & 0x8000;

            r = ((r << 16) + scale_a - (scale_b * r)) >> 16;
            g = ((g << 16) + scale_a - (scale_b * g)) >> 16;
            b = ((b << 16) + scale_a - (scale_b * b)) >> 16;
            *dst = alpha | r | (g << 5) | (b << 10);
        } else {
            *dst = 0;
        }
        dst++;
    }

    level = scale_a >> 13;
    D_800BCE3F = level;
    D_800BCE3E = level;
    D_800BCE3D = level;
    D_800BCDE3 = level;
    D_800BCDE2 = level;
    D_800BCDE1 = level;
}
