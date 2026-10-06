/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSSYS_1.OBJ, part 5 of 11: LIBDS_DSSYS_1_text_368, LIBDS_DSSYS_1_text_4A4, LIBDS_DSSYS_1_text_774, LIBDS_DSSYS_1_text_7FC. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

extern void parcpy(void *, const void *);

int LIBDS_DSSYS_1_text_368(unsigned char command, void *param) {
    unsigned char *state;
    int *timeout;
    CD_flush();
    state = (unsigned char *)&g_CdRomEventCommandState;
    state[0] = command;
    if (param) {
        unsigned char *copy = state + 1;
        parcpy(copy, param);
        *(void **)(state + 8) = copy;
    } else *(void **)(state + 8) = 0;
    timeout = &g_CdRomCmdTimeout;
    timeout[0] = g_CdRomCmdLongTimeoutTable[((unsigned char *)timeout)[-64]] ? 960 : 30;
    timeout[1] = 0;
    switch (((unsigned char *)timeout)[-64]) {
    case 7:
        if (((unsigned char *)timeout)[-13] == 1) {
            ((unsigned char *)timeout)[-64] = 1;
            timeout[-14] = 0;
        }
        break;
    case 8:
        if (((unsigned char *)timeout)[-13] != 1) {
            ((unsigned char *)timeout)[-64] = 1;
            timeout[-14] = 0;
        }
        break;
    }
    {
        unsigned char *current = (unsigned char *)&g_CdRomEventCommandState;
        if (CD_cw(current[0], *(void **)(current + 8), 0, 1)) {
            *(int *)(current + 68) = 0;
            *(int *)(current + 64) = 0;
            return 0;
        } else {
            current[40] = current[0];
            return 1;
        }
    }
}
s32 LIBDS_DSSYS_1_text_774(void);
int LIBDS_DSSYS_1_text_368(unsigned char, void *);
#define READ(p)                                                                        \
    ((DsReadStatusBlock *)((u8 *)(p) - PE1_OFFSETOF(DsReadStatusBlock, syncResult)))
#define CMD(p)                                                                         \
    ((CdRomCommandState *)((u8 *)(p) -                                                 \
                           PE1_OFFSETOF(CdRomCommandState, read.syncResult)))
void LIBDS_DSSYS_1_text_4A4(void) {
    u8 parameter;
    register int pending;
    register int one;
    register int *timer;
    register int *retry = &g_CdRomCmdTimeout;
    if (*retry > 0 && --*retry == 0) {
        LIBDS_DSSYS_1_text_774();
        goto done;
    }
    timer = &g_DsSyncResultCountdown;
        if (*timer > 0)
        --*timer;
    if ((int)CMD(timer)->reserved34 > 0)
        --CMD(timer)->reserved34;
    one = 1;
    if (READ(timer)->status != one && READ(timer)->status == 2) {
        register int state = READ(timer)->command;
        register int eleven = 11;
        if (state == eleven)
            goto callbacks;
        if (state == 12) {
            if (g_CdReadCommandPollToggle) {
                register u8 *arg = &parameter;
                parameter = 0;
                if (g_CdRomCmdTimeout <= 0) {
                    CMD(timer)->eventValue = 32;
                    LIBDS_DSSYS_1_text_368(14, arg);
                }
                g_CdReadCommandPollToggle = 0;
            } else {
                if (g_CdRomCmdTimeout <= 0) {
                    CMD(timer)->eventValue = 32;
                    LIBDS_DSSYS_1_text_368(1, 0);
                }
                g_CdReadCommandPollToggle = one;
            }
        } else if (state == 13) {
            pending = g_CdRomCmdTimeout;
            g_CdReadCommandPollToggle = 0;
            goto check_timeout;
        } else if (state == 14) {
            int step = READ(timer)->sector;
            if (step == 21)
                goto poll;
            else if (step == 22) {
                READ(timer)->reserved18++;
                if (g_CdRomCmdTimeout > 0)
                    goto callbacks;
                goto send_status;
            } else if (step == 23) {
                if (g_CdRomCmdTimeout <= 0) {
                    CMD(timer)->eventValue = 32;
                    LIBDS_DSSYS_1_text_368(19, 0);
                }
            } else if (step == 24)
                goto poll;
        } else if (state == 15) {
            if (!*timer) {
                READ(timer)->status = one;
                goto complete;
            }
        } else if (state == 16) {
            if (!READ(timer)->eventFlags.bit7)
                goto poll;
            asm("" ::: "memory");
            READ(timer)->status = one;
            goto complete;
        } else if (state == 17) {
            if (!READ(timer)->eventFlags.bit5)
                goto poll;
            READ(timer)->status = one;
        complete:
            READ(timer)->command = eleven;
        }
        goto callbacks;
    poll:
        pending = g_CdRomCmdTimeout;
    check_timeout:
        if (pending > 0)
            goto callbacks;
    send_status:
        CMD(timer)->eventValue = 32;
        LIBDS_DSSYS_1_text_368(1, 0);
    }
callbacks:
    if (g_DsPollCallback && g_DsReadSysEnabled.enabled)
        g_DsPollCallback();
    {
        DsReadStatusBlock *read = &g_DsReadStatusBlock;
                if ((read->status == 1 && !read->eventFlags.bit1) || read->status == 3) {
            if (g_CdRomCmdTimeout <= 0) {
                g_CdRomCommandEventValue = 33;
                LIBDS_DSSYS_1_text_368(1, 0);
            }
        }
    }
done:;
}

void CD_flush(void);

extern s32 D_8009B59C[];
#define D_8009B59C (D_8009B59C[0])

s32 LIBDS_DSSYS_1_text_774(void) {
    void *base;
    s32 value;
    register s32 idx asm("$3");
    s32 arg0;
    s32 arg2;
    s32 arg3;

    CD_flush();
    base = &D_8009B59C;
        value = *(s32 *)base;
    idx = *(u8 *)((char *)base - 0x44);
    value += 1;
    idx = idx << 2;
    *(s32 *)base = value;
        {
        /* g_CdRomCmdLongTimeoutTable is at 0x8009B5A4 in the USA image. */
        u32 table_page = 0x800A0000u;
        asm volatile("" : "=r"(table_page) : "0"(table_page));
        value = *(s32 *)(table_page + idx - 0x4A5Cu);
    }
    idx = 0x1E;
    if (value != 0) {
        idx = 0x3C0;
    }
    arg0 = *(u8 *)((char *)base - 0x44);
    arg2 = 0;
    *(s32 *)((char *)base - 4) = idx;
    value = *(s32 *)((char *)base - 0x3C);
    arg3 = 1;
    CD_cw(arg0, (void *)value, (u8 *)arg2, arg3);
    return 0;
}

extern int D_8009B598[];
extern void LIBDS_DSSYS_1_text_EA4(int);
extern void LIBDS_DSSYS_1_text_8B8(int, void *);
extern void LIBDS_DSSYS_1_text_A9C(int, void *);

void LIBDS_DSSYS_1_text_7FC(int inputEvent, void *inputResult) {
    void *result = inputResult;
    int event = inputEvent;
    int *state;

    LIBDS_DSSYS_1_text_EA4((unsigned char)event);
    /* The timeout symbol anchors the surrounding CD command state. */
    state = D_8009B598;
    state[1] = 0;
    state[0] = 0;
    if (((unsigned char *)state)[-44] & 0x10) event = 5;
    switch (state[-10]) {
    case 0x1F:
        LIBDS_DSSYS_1_text_8B8((unsigned char)event, result);
        break;
    case 0x20:
        LIBDS_DSSYS_1_text_A9C((unsigned char)event, result);
        break;
    default:
        LIBDS_DSSYS_1_text_D24((unsigned char)event, result);
        break;
    }
    state = D_8009B598;
    if (!state[0]) state[-10] = 0x21;
}
