/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned char D_8009CEB4;
short D_8009CEB8;
short D_8009CEBC;
short D_8009CEC0;
unsigned char D_8009CEC4;
unsigned char D_8009CEC8;
unsigned char D_8009CECC;

void Gpu_SetLightingParams(short arg0, short arg1, short arg2, unsigned char arg3, unsigned char arg4, unsigned char arg5) {
    D_8009CEB8 = arg0;
    D_8009CEB4 = 1;
    D_8009CEBC = arg1;
    D_8009CEC0 = arg2;
    D_8009CEC4 = arg3;
    D_8009CEC8 = arg4;
    D_8009CECC = arg5;
}
