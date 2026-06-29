/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int **g_PmCmdHandlerTable;
extern int g_GameState;
extern int D_800E10A0[];

int Pm_Exec(int arg0) {
    register int offset asm("$2");
    char *entry;
    int state;
    int cmd;
    int **handler;
    int (*callback)(char *);
    int **table;
    int table_offset;
    unsigned int i;

    if ((unsigned int)arg0 >= 0x16) {
        return -0x13;
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

    state = *(u8 *)entry;
    if (((unsigned int)(state - 1) >= 2U) && ((unsigned int)(state - 4) >= 2U)) {
        return 0;
    }

    {
        int state2;
        asm volatile("" ::: "memory");
        state2 = *(u8 *)entry;
        state = state2;
    }
    if (state == 4) {
        *(u8 *)entry = 5;
        return 0;
    }
    if (state == 5) {
        if ((unsigned int)arg0 < 0x16) {
            char *cleanup;

            if ((unsigned int)arg0 >= 0xB) {
                int idx;
                int offset_hi;
                idx = arg0 - 0xB;
                offset_hi = idx << 4;
                offset_hi += idx;
                offset_hi <<= 2;
                offset_hi -= idx;
                cleanup = g_PmSlotTable2 + (offset_hi << 2);
            } else {
                offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
                offset <<= 2;
                cleanup = g_PmSlotTable + offset;
            }
            if (*(u8 *)(cleanup + 1) == 0x72) {
                for (i = 0x6C; i < 0x73; i++) {
                    D_800E10A0[i - 0x6C] = 0;
                }
                g_GameState &= 0xFFFEFFFF;
            }
            *(u8 *)cleanup = 0;
            *(u8 *)(cleanup + 1) = -1;
            *(u8 *)(cleanup + 2) = -1;
            *(u8 *)(cleanup + 3) = -1;
            *(int *)(cleanup + 4) = 0;
            *(int *)(cleanup + 8) = 0;
        }
        return 0;
    }

    if (state == 1) {
        *(u8 *)entry = 2;
    }

    cmd = *(u8 *)(entry + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0x14;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(int ***)(table_offset + (int)table);
    if (handler == 0) {
        return -0x15;
    }
    callback = (int (*)(char *))handler[4];
    if (callback != 0) {
        int ret = callback(entry);
        *(int *)(entry + 4) = *(int *)(entry + 4) + 1;
        return ret;
    }

    return -1;
}
