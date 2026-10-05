/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
extern void Util_Copy4(void *, const void *);

int CdRom_SendCmd(unsigned char command, void *param) {
    unsigned char *state;
    int *timeout;
    CD_flush();
    state = (unsigned char *)&g_CdRomEventCommandState;
    state[0] = command;
    if (param) {
        unsigned char *copy = state + 1;
        Util_Copy4(copy, param);
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
/* ASSEMBLER: GNU */
#include "pe1/psyq_ds.h"
void CdRom_RetryCmd(void);
int CdRom_SendCmd(unsigned char, void *);
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
        CdRom_RetryCmd();
        goto done;
    }
    timer = &g_DsSyncResultCountdown;
    asm("" : "=r"(timer) : "0"(timer));
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
                    CdRom_SendCmd(14, arg);
                }
                g_CdReadCommandPollToggle = 0;
            } else {
                if (g_CdRomCmdTimeout <= 0) {
                    CMD(timer)->eventValue = 32;
                    CdRom_SendCmd(1, 0);
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
                    CdRom_SendCmd(19, 0);
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
        CdRom_SendCmd(1, 0);
    }
callbacks:
    if (g_DsPollCallback && g_DsReadSysEnabled.enabled)
        g_DsPollCallback();
    {
        register DsReadStatusBlock *read asm("$4") = &g_DsReadStatusBlock;
        asm("" : "=r"(read) : "0"(read));
        if ((read->status == 1 && !read->eventFlags.bit1) || read->status == 3) {
            if (g_CdRomCmdTimeout <= 0) {
                g_CdRomCommandEventValue = 33;
                CdRom_SendCmd(1, 0);
            }
        }
    }
done:;
}
