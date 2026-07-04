/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned char *g_SaveMetadataCursor;

unsigned char *Save_GetMetadataCursor(void) {
    return g_SaveMetadataCursor;
}
