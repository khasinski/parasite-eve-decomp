/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_SaveMetadataWindowIndex;

void Save_ResetMetadataWindow(void) {
    g_SaveMetadataWindowIndex = 1;
}
