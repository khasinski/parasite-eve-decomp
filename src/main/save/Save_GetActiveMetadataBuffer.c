/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SaveMetadataWindowIndex;
extern char g_SaveRuntimeState[];

char *Save_GetActiveMetadataBuffer(void) {
    char *buffer;

    buffer = g_SaveRuntimeState;

    if (g_SaveMetadataWindowIndex != 0) {
        buffer += 0x10;
    }

    return buffer;
}
