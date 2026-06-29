/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int **g_PmCmdHandlerTable;

int Pm_SetGetState(int arg0, int arg1, int arg2) {
    int offset;
    int cmd;
    int **handler;

    if ((unsigned int)arg0 >= 0x16) {
        return -0xD;
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

    cmd = *(u8 *)(arg0 + 1);
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    handler = (int **)g_PmCmdHandlerTable[cmd];
    if (handler == 0) {
        return -0xF;
    }

    if (arg1 == 0) {
        if ((unsigned int)arg2 < 6) {
            *(u8 *)arg0 = arg2;
        }
    } else {
        *(int *)arg2 = *(u8 *)arg0;
    }

    return *(u8 *)arg0;
}
