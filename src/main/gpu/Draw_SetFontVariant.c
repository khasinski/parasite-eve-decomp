/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_DrawDigitFontTpageClut;
extern int g_DrawDigitFontBaseTexU;
extern int g_DrawDigitFontBaseTexV;

void Draw_SetFontVariant(int arg0) {
    g_DrawDigitFontTpageClut = arg0 != 0 ? 0x3A1C : 0x395D;
    g_DrawDigitFontBaseTexU = arg0 != 0 ? 0xCC : 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;
}
