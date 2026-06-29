/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned char D_8009CEC4;
unsigned char D_8009CEC8;
unsigned char D_8009CECC;

void Menu_SetHighlightColor(int arg0, int arg1, int arg2, int arg3) {
    D_8009CEC4 = arg1;
    D_8009CEC8 = arg2;
    D_8009CECC = arg3;
}
