/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_McDialogMode;
int g_MenuActiveWidget;

unsigned char *MemCard_GetSlot(int arg0, int arg1);
int Save_GetTitleStyleFlag(void);

int Menu_IsMemCardSlotCompatible(int arg0) {
    unsigned char *ptr;
    int result;

    if (g_McDialogMode != 0) {
        ptr = MemCard_GetSlot(*(int *)(g_MenuActiveWidget + 0x24) - 0x25, arg0);
        result = ptr[0] != 3;
    } else {
        ptr = MemCard_GetSlot(*(int *)(g_MenuActiveWidget + 0x24) - 0x25, arg0);
        if (ptr[0] != 1) {
            result = 0;
            goto done;
        }

        result = ptr[0x29];
        result ^= Save_GetTitleStyleFlag();
        result = (unsigned int)result < 1;
    }

done:
    return result;
}
