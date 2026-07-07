/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_InvPackedListCount;

int g_InvPackedListCursor;

int Inv_GetPackedListCount(void) {
    return g_InvPackedListCount;
}

int Inv_GetPackedListCursor(void) {
    return g_InvPackedListCursor;
}
