/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvPackedListCount;

int Inv_GetPackedListCount(void) {
    return g_InvPackedListCount;
}
