/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned char g_MenuActiveMode;

unsigned int Menu_GetActiveMode(void) {
    return g_MenuActiveMode;
}
