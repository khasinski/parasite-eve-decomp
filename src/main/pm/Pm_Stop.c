/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int **g_PmCmdHandlerTable;
extern int g_GameState;
extern int D_800E10A0[];

int Pm_Stop(int arg0, int arg1, int arg2) {
    register int state asm("$3");
    int **handler;
    int (*callback)(void);
    int result;
    register int offset asm("$2");
    int orig;
    unsigned int i;

    orig = arg0;
    if ((unsigned int)orig >= 0x16) {
        return -0x16;
    }

    if ((unsigned int)orig >= 0xB) {
        int idx;
        int offset_hi;
        idx = orig - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)(g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((orig * 4) + orig) << 5) + orig) << 2) - orig;
        offset <<= 2;
        arg0 = (int)(g_PmSlotTable + offset);
    }
    state = *(u8 *)arg0;
    if ((state == 0) || (state == 6)) {
        return 0;
    }
    if (arg2 == 0) {
        if (*(int *)(arg0 + 8) != arg1) {
            return 0;
        }
    }

    arg1 = *(u8 *)(arg0 + 1);
    if ((unsigned int)arg1 >= 0xC0) {
        return -0x17;
    }
    if ((unsigned int)arg1 >= 0x55) {
        arg1 = 0x55;
    }

    handler = (int **)g_PmCmdHandlerTable[arg1];
    if (handler == 0) {
        return -0x18;
    }
    callback = (int (*)(void))handler[5];
    if (callback == 0) {
        return -1;
    }

    result = callback();

    if ((unsigned int)orig < 0x16) {
        if ((unsigned int)orig >= 0xB) {
            int idx;
            int offset_hi;
            idx = orig - 0xB;
            offset_hi = idx << 4;
            offset_hi += idx;
            offset_hi <<= 2;
            offset_hi -= idx;
            arg1 = (int)(g_PmSlotTable2 + (offset_hi << 2));
        } else {
            offset = (((((orig * 4) + orig) << 5) + orig) << 2) - orig;
            offset <<= 2;
            arg1 = (int)(g_PmSlotTable + offset);
        }
        if (*(u8 *)(arg1 + 1) == 0x72) {
            for (i = 0x6C; i < 0x73; i++) {
                D_800E10A0[i - 0x6C] = 0;
            }
            g_GameState &= 0xFFFEFFFF;
        }
        *(u8 *)arg1 = 0;
        *(u8 *)(arg1 + 1) = -1;
        *(u8 *)(arg1 + 2) = -1;
        *(u8 *)(arg1 + 3) = -1;
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 8) = 0;
    }

    return result;
}
