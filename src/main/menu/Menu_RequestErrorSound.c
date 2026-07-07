/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned int g_MenuErrorSoundPending;

void Menu_RequestErrorSound(void) {
    g_MenuErrorSoundPending = 1;
}
