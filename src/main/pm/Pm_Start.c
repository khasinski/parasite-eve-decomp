/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int **g_PmCmdHandlerTable;

int Pm_Start(int arg0) {
    int offset;
    register int cmd asm("$5");
    int **handler;
    int (*callback)(void);
    int **table;
    register int table_offset asm("$2");

    if ((unsigned int)arg0 >= 0x16) {
        return -0x10;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)(g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        arg0 = (int)(g_PmSlotTable + offset);
    }

    if ((unsigned int)(*(u8 *)arg0 - 1) >= 2U) {
        return 0;
    }

    cmd = *(u8 *)(arg0 + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0x11;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(int ***)(table_offset + (int)table);
    if (handler == 0) {
        return -0x12;
    }
    callback = (int (*)(void))handler[3];
    if (callback != 0) {
        return callback();
    }

    return -1;
}
