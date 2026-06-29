typedef unsigned char u8;

u8 *Item_LookupBaseData(int index);

void Item_SetDisabledFlag(int arg0, int arg1) {
    u8 *entry;
    int flags;

    entry = Item_LookupBaseData(arg0 - 1);
    if (entry != 0) {
        flags = entry[5] & 0xBF;
        entry[5] = flags;
        if (arg1 == 0) {
            flags |= 0x40;
        }
        entry[5] = flags;
    }
}
