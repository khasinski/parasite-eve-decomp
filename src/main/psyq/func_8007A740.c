/* MASPSX_FLAGS: --store-return-delay --store-branch-delay --stack-return-delay */
/* CC1_FLAGS: -fno-schedule-insns */

extern int CD_sync(int arg0, void *arg1);
extern int CD_cw(int arg0, void *arg1, int arg2, int arg3);
extern int D_8009AF2C[];
extern int D_8009AFB4;
extern unsigned char D_8009AFC4;

int func_8007A740(int arg0, void *arg1, void *arg2) {
    register void *arg1_reg asm("$17");
    register void *arg2_reg asm("$18");
    register int arg0_reg asm("$20");
    register int retries asm("$16");
    int one;
    register int cmd asm("$19");
    register int saved_callback asm("$21");
    register int *slot asm("$22");
    register int minus_one asm("$23");
    register int *slot_base asm("$3");
    register int slot_offset asm("$2");
    register int result asm("$2");
    register int zero_arg asm("$4");

    arg1_reg = arg1;
    arg2_reg = arg2;
    arg0_reg = arg0;
    retries = 3;
    one = 1;
    cmd = arg0_reg & 0xFF;
    slot_base = D_8009AF2C;
    saved_callback = D_8009AFB4;
    slot_offset = cmd << 2;
    slot = (int *)((char *)slot_base + slot_offset);
    minus_one = -1;

    do {
        D_8009AFB4 = 0;
        if (cmd == one) {
            goto maybe_queue;
        }
        if (D_8009AFC4 & 0x10) {
            CD_cw(1, 0, 0, 0);
        }

maybe_queue:
        if (arg1_reg != 0 && *slot != 0) {
            if (CD_cw(2, arg1_reg, (int)arg2_reg, 0) != 0) {
                goto retry;
            }
        }

        D_8009AFB4 = saved_callback;
        if (CD_cw(arg0_reg & 0xFF, arg1_reg, (int)arg2_reg, 0) == 0) {
            result = 0;
            goto sync;
        }

retry:
        retries--;
    } while (retries != minus_one);

    D_8009AFB4 = saved_callback;
    result = -1;

sync:
    zero_arg = 0;
    if (result != 0) {
        return 0;
    }

    return CD_sync(zero_arg, arg2_reg) == 2;
}
