/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int D_8009D110;
extern int D_8009D114;
extern int D_8009D124;
extern int D_8009D128;
extern int D_8009D164;
extern int D_8009D168;

void Draw_AllocColorTri(int width, int height, int arg2);

void Draw_DropShadow(void) {
    D_8009D110 = 0xFF4000;
    D_8009D114 = 0x7F2000;
    D_8009D124 -= 2;
    D_8009D128 -= 2;
    Draw_AllocColorTri(D_8009D164, D_8009D168, 0);
    D_8009D110 = 0x808080;
    D_8009D114 = 0x404040;
    D_8009D124 += 2;
    D_8009D128 += 2;
}
