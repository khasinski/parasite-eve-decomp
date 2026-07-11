/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CED8;

int Menu_SaveBgIsFadeActive(void) {
    return D_8009CED8 != 0;
}
