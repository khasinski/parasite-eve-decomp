/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

extern int CD_cw(int arg0, void *arg1, int arg2, int arg3);
extern int D_8009AF2C[];
extern int g_CdSyncCallback;
extern unsigned char g_CdStatus;

int func_8007A60C(int arg0, void *arg1) {
    void *arg1_reg;
    int arg0_reg;
    int retries;
    int cmd;
    int saved_callback;
    int *slot;
    int result;
    int *slot_base;
    int slot_offset;
    int minus_one;
    int one;

    saved_callback = g_CdSyncCallback;
    arg1_reg = arg1;
    arg0_reg = arg0;
    retries = 3;
    one = 1;
    cmd = arg0_reg & 0xFF;
    slot_base = D_8009AF2C;
    slot_offset = cmd << 2;
    slot = (int *)((char *)slot_base + slot_offset);
    result = 0;
    minus_one = -1;

    for (;;) {
        g_CdSyncCallback = 0;
        if (cmd != one && (g_CdStatus & 0x10)) {
            CD_cw(1, 0, 0, 0);
        }

        if (arg1_reg == 0 || *slot == 0 || CD_cw(2, arg1_reg, 0, 0) == 0) {
            g_CdSyncCallback = saved_callback;
            if (CD_cw(arg0_reg & 0xFF, arg1_reg, 0, 1) == 0) {
                break;
            }
        }

        retries--;
        if (retries == minus_one) {
            g_CdSyncCallback = saved_callback;
            result = -1;
            break;
        }
    }

    return result + 1;
}
