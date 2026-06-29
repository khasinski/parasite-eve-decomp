/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;

int Pm_AllocSlot(unsigned int cmd) {
    int result;
    int i;
    char *entry;

    result = -1;
    if (cmd >= 0xC0) {
        return -1;
    }

    if ((unsigned int)(cmd - 0x46) < 0xF) {
        i = 0;
        entry = g_PmSlotTable2;
        do {
            if (*(u8 *)entry == 0) {
                result = i + 0xB;
                goto done;
            }
            i++;
            entry += 0x10C;
        } while (i < 0xB);
        return result;
    }

    i = 0;
    entry = g_PmSlotTable;
    do {
        if (*(u8 *)entry == 0) {
            result = i;
            goto done;
        }
        i++;
        entry += 0xA0C;
    } while (i < 0xB);

done:
    return result;
}
