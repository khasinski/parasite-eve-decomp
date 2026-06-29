/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvPackedListCursor;

int Inv_GetPackedListCursor(void) {
    return g_InvPackedListCursor;
}
