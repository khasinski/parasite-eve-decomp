/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CDAC;
extern int D_8009D0D4;

void Menu_InitScrollDirection(void) {
    D_8009D0D4 = D_8009CDAC < 0x12C ? 4 : -4;
}
