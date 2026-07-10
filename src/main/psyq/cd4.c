/* MASPSX_FLAGS: --store-return-delay --store-branch-delay --stack-return-delay */
/* CC1_FLAGS: -fno-schedule-insns */
#include "include_asm.h"
#include "pe1/psyq_cd.h"

extern int CD_sync();

extern void CD_ready(void);

extern int g_CdSyncCallback;

extern int g_CdReadyCallback;

extern void CD_vol(CdlATV *vol);

extern int CD_getsector2(void);

extern int CD_getsector(void);

void DMACallback(int arg0, int arg1);

extern void CD_datasync(void);
extern int CD_cw(int arg0, void *arg1, int arg2, int arg3);
extern int D_8009AF2C[];
extern int D_8009AFB4;
extern unsigned char D_8009AFC4;

void CdSync(void) {
    CD_sync();
}

void CdReady(void) {
    CD_ready();
}

int CdSyncCallback(int callback) {
    int old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}

int CdReadyCallback(int callback) {
    int old = g_CdReadyCallback;
    g_CdReadyCallback = callback;
    return old;
}

int func_8007A4D0(int arg0, void *arg1, void *arg2) {
    register void *arg1_reg asm("$17");
    register void *arg2_reg asm("$18");
    register int arg0_reg asm("$20");
    register int retries asm("$16");
    register int cmd asm("$19");
    register int saved_callback asm("$21");
    register int *slot asm("$22");
    register int result asm("$23");
    register int *slot_base asm("$3");
    register int slot_offset asm("$2");
    int minus_one;
    register int one asm("$8");

    arg1_reg = arg1;
    arg2_reg = arg2;
    arg0_reg = arg0;
    retries = 3;
    cmd = arg0_reg & 0xFF;
    slot_base = D_8009AF2C;
    saved_callback = D_8009AFB4;
    slot_offset = cmd << 2;
    slot = (int *)((char *)slot_base + slot_offset);
    result = 0;
    minus_one = -1;

    do {
        D_8009AFB4 = 0;
        one = 1;
        if (cmd != one && (D_8009AFC4 & 0x10)) {
            CD_cw(1, 0, 0, 0);
        }

        if (arg1_reg != 0 && *slot != 0) {
            if (CD_cw(2, arg1_reg, (int)arg2_reg, 0) != 0) {
                goto retry;
            }
        }

        D_8009AFB4 = saved_callback;
        if (CD_cw(arg0_reg & 0xFF, arg1_reg, (int)arg2_reg, 0) == 0) {
            goto out;
        }

retry:
        retries--;
    } while (retries != minus_one);

    D_8009AFB4 = saved_callback;
    result = -1;
out:
    return result + 1;
}

int func_8007A60C(int arg0, void *arg1) {
    register void *arg1_reg asm("$17");
    register int arg0_reg asm("$19");
    register int retries asm("$16");
    register int cmd asm("$18");
    register int saved_callback asm("$20");
    register int *slot asm("$21");
    register int result asm("$22");
    register int minus_one asm("$23");
    register int *slot_base asm("$3");
    register int slot_offset asm("$2");
    int one;

    arg1_reg = arg1;
    arg0_reg = arg0;
    retries = 3;
    one = 1;
    cmd = arg0_reg & 0xFF;
    slot_base = D_8009AF2C;
    saved_callback = D_8009AFB4;
    slot_offset = cmd << 2;
    slot = (int *)((char *)slot_base + slot_offset);
    result = 0;
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
            if (CD_cw(2, arg1_reg, 0, 0) != 0) {
                goto retry;
            }
        }

        D_8009AFB4 = saved_callback;
        if (CD_cw(arg0_reg & 0xFF, arg1_reg, 0, 1) == 0) {
            goto out;
        }

retry:
        retries--;
    } while (retries != minus_one);

    D_8009AFB4 = saved_callback;
    result = -1;
out:
    return result + 1;
}

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

int CdMix(CdlATV *vol) {
    CD_vol(vol);
    return 1;
}

int CdGetSector2(void) {
    return CD_getsector2() == 0;
}

int CdGetSector(void) {
    return CD_getsector() == 0;
}

void CdDataCallback(int arg) {
    DMACallback(3, arg);
}

void CdDataSync(void) {
    CD_datasync();
}

static inline int ENCODE_BCD(int n) {
    return ((n / 10) << 4) + (n % 10);
}

CdlLOC *CdIntToPos_Local(int i, CdlLOC *p) {
    i += 150;
    p->sector = ENCODE_BCD(i % 75);
    p->second = ENCODE_BCD(i / 75 % 60);
    p->minute = ENCODE_BCD(i / 75 / 60);
    return p;
}

int CdPosToInt_Local(CdlLOC *p) {
#define DECODE_BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
    u_char sector = p->sector;
    u_char second = p->second;
    u_char minute = p->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}
