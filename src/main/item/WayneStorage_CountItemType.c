/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern short g_WayneStorageItems[];

void *Item_LookupBaseData(int arg0);

int WayneStorage_CountItemType(int arg0) {
    short *item;
    int index;
    int count;
    int itemId;

    count = 0;
    index = 0;
    item = g_WayneStorageItems;
    do {
        itemId = *item;
        if (itemId != 0) {
            if (((unsigned char *)Item_LookupBaseData(itemId - 1))[6] == arg0) {
                count++;
            }
        }
        index++;
        item++;
    } while (index < 0x64);

    return count;
}
