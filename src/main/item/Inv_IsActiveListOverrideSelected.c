/* MASPSX_FLAGS: -G8 --use-comm-section */

void *g_InvItemPtr;
extern char g_AyaInventoryItems[];

int Inv_IsActiveListOverrideSelected(void) {
    return g_InvItemPtr != g_AyaInventoryItems;
}
