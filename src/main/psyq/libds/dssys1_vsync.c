/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"
extern int D_8009B574, D_8009B594, D_8009B598, D_8009B6A4, D_8009B570;
extern DsCallback volatile D_800A36A0;
void CdRom_RetryCmd(void);
int CdRom_SendCmd(int, void *);
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
    register int *retry = &D_8009B598;
    if (*retry > 0 && --*retry == 0) {
        CdRom_RetryCmd();
        goto done;
    }
    timer = &D_8009B594;
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
            if (D_8009B6A4) {
                register u8 *arg = &parameter;
                parameter = 0;
                if (D_8009B598 <= 0) {
                    CMD(timer)->eventValue = 32;
                    CdRom_SendCmd(14, arg);
                }
                D_8009B6A4 = 0;
            } else {
                if (D_8009B598 <= 0) {
                    CMD(timer)->eventValue = 32;
                    CdRom_SendCmd(1, 0);
                }
                D_8009B6A4 = one;
            }
        } else if (state == 13) {
            pending = D_8009B598;
            D_8009B6A4 = 0;
            goto check_timeout;
        } else if (state == 14) {
            int step = READ(timer)->sector;
            if (step == 21)
                goto poll;
            else if (step == 22) {
                READ(timer)->reserved18++;
                if (D_8009B598 > 0)
                    goto callbacks;
                goto send_status;
            } else if (step == 23) {
                if (D_8009B598 <= 0) {
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
        pending = D_8009B598;
    check_timeout:
        if (pending > 0)
            goto callbacks;
    send_status:
        CMD(timer)->eventValue = 32;
        CdRom_SendCmd(1, 0);
    }
callbacks:
    if (D_800A36A0 && g_DsReadSysEnabled.enabled)
        D_800A36A0();
    {
        register DsReadStatusBlock *read asm("$4") = (DsReadStatusBlock *)&D_8009B574;
        asm("" : "=r"(read) : "0"(read));
        if ((read->status == 1 && !read->eventFlags.bit1) || read->status == 3) {
            if (D_8009B598 <= 0) {
                D_8009B570 = 33;
                CdRom_SendCmd(1, 0);
            }
        }
    }
done:;
}
