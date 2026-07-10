/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;

extern int D_8009D0D8;
extern int D_8009CDB0;

int Draw_LookupGlyphMetrics(int glyph);

int Draw_MeasureTextWidth(u8 *text) {
    register u8 *cursor asm("$16");
    register int width asm("$17");
    register int ch asm("$4");
    int glyph;
    register int terminator_check asm("$3");
    register int sentinel asm("$2");
    int spacing;
    int metrics;

    cursor = text;
    width = 0;
    goto test;
loop:
    glyph = ch & 0xFF;
    if (D_8009D0D8 != 0) {
        glyph += D_8009D0D8 << 8;
        D_8009D0D8 = 0;
    }
    ch &= 0xFF;
    if ((unsigned int)ch >= 0xFA) {
        D_8009D0D8 = ch - 0xFA;
        glyph = -1;
    }
    ch = glyph;
    if (ch >= 0) {
        spacing = 0;
        if (ch < 10 || ch == 15) {
            spacing = 1;
        }
        D_8009CDB0 = spacing + 1;
        if (ch >= 0x100) {
            ch -= 0x13;
        }
        metrics = Draw_LookupGlyphMetrics(ch);
        width += ((metrics >> 4) & 0xF) + D_8009CDB0;
    }
test:
    ch = *cursor;
    sentinel = 0xFF;
    asm volatile("andi %0,%1,0xFF" : "=r"(terminator_check) : "r"(ch));
    if (terminator_check != sentinel) {
        cursor++;
        goto loop;
    }
    asm volatile("addiu $16,$16,-1");
    return width;
}
