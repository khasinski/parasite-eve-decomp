/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int D_8009CED8;
extern int D_8009CEDC;
extern int D_8009CEE0;
extern int D_8009CEE4;
extern int D_8009CEEC;

void Menu_ComputeGammaLut(int arg0, int arg1);

void Menu_SaveBgInitFade(void) {
    D_8009CED8 = 0;
    D_8009CEE0 = 0;
    D_8009CEE4 = 0;
    D_8009CEDC = 0x20;
    Menu_ComputeGammaLut(0x90, 0xFF);
    D_8009CEEC = 0x48;
}
