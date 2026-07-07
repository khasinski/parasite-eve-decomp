/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned int g_McDialogMode;

unsigned int MemCard_GetDialogMode(void) {
    return g_McDialogMode;
}
