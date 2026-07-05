/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SaveMetadataWindowIndex;

void Save_PostInitStub(void) {
}

int Save_GetMetadataWindowIndex(void) {
    return g_SaveMetadataWindowIndex;
}
