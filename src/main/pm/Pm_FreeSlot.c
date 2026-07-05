typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int g_GameState;
extern int D_800E10A0[];

int Pm_FreeSlot(int arg0) {
    int offset;
    char *entry;
    unsigned int i;

    if ((unsigned int)arg0 >= 0x16) {
        return -1;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        entry = g_PmSlotTable2 + (offset_hi << 2);
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = g_PmSlotTable + offset;
    }

    if ((u8)entry[1] == 0x72) {
        for (i = 0x6C; i < 0x73; i++) {
            D_800E10A0[i - 0x6C] = 0;
        }
        {
            int *state = &g_GameState;
            *state &= 0xFFFEFFFF;
        }
    }

    entry[0] = 0;
    entry[1] = -1;
    entry[2] = -1;
    entry[3] = -1;
    *(int *)(entry + 4) = 0;
    *(int *)(entry + 8) = 0;

    return 0;
}
