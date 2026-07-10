#include "include_asm.h"

#include "pe1/akao.h"

int Spu_ValidateSampleHeader();

extern volatile int D_8009D2E0;

int Spu_UploadSampleBlock(int arg0, int arg1);

extern int g_SpuTransferStatus;

extern int g_AkaoPlaybackMode;
extern unsigned char D_8009D1C8;
extern unsigned char D_8009D1C9;
extern unsigned char D_8009D1CA;
extern unsigned char D_8009D1CB;

void CdMix(void *arg0);

extern int g_AkaoStreamUploadMode;

#include "include_asm.h"

extern volatile unsigned short D_1F801D88;
extern volatile unsigned short D_1F801D8A;

extern volatile unsigned short D_1F801D8C;
extern volatile unsigned short D_1F801D8E;

extern volatile unsigned short D_1F801D98;
extern volatile unsigned short D_1F801D9A;

extern volatile unsigned short D_1F801D94;
extern volatile unsigned short D_1F801D96;

extern volatile unsigned short D_1F801D90;
extern volatile unsigned short D_1F801D92;

void Akao_Cmd_10(int arg0) {
    g_AkaoCmdOpcode = 0x10;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_11(int arg0) {
    g_AkaoCmdOpcode = 0x11;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_40(void) {
    g_AkaoCmdOpcode = 0x40;
    Akao_EnqueueStagedCommand();
}

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    int *opcode = &g_AkaoCmdOpcode;
    int ret;

    *opcode = 0x19;
    g_AkaoCmdArg0 = arg0;
    ret = Akao_EnqueueStagedCommand();

    *opcode = AKAO_SEQ_OP_TRANSPOSE_ABSOLUTE;
    g_AkaoCmdArg0 = arg1 & 0x7F;
    g_AkaoCmdArg3 = 0;
    Akao_EnqueueStagedCommand();

    return ret;
}

void Akao_Cmd_12(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0x12;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_20(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0x20;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3) {
    if (Spu_ValidateSampleHeader() != 0) {
        return;
    }

    g_AkaoCmdOpcode = 0x24;
    g_AkaoCmdArg0 = arg0 + 4;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_21(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0x21;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_30(int arg0) {
    g_AkaoCmdOpcode = 0x30;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_80_81_82(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x81;
        break;
    case 2:
        opcode = 0x82;
        break;
    default:
        opcode = 0x80;
        break;
    }

    g_AkaoCmdOpcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_90(int arg0) {
    g_AkaoCmdOpcode = 0x90;
    g_AkaoCmdArg0 = arg0 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_92(int arg0) {
    g_AkaoCmdOpcode = 0x92;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_99_9B_9D(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9B;
        break;
    case 2:
        opcode = 0x9D;
        break;
    default:
        opcode = 0x99;
        break;
    }

    g_AkaoCmdOpcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_98_9A_9C(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9A;
        break;
    case 2:
        opcode = 0x9C;
        break;
    default:
        opcode = 0x98;
        break;
    }

    g_AkaoCmdOpcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A8(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_CHANNEL_VOLUME;
    g_AkaoCmdArg0 = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_CHANNEL_VOLUME_SLIDE;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A0(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_FINISH_CHANNEL;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_LOAD_INSTRUMENT;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AA(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_CHANNEL_PAN;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AB(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_CHANNEL_PAN_SLIDE;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A2(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_SET_NEXT_NOTE_LENGTH;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_CHANNEL_MASTER_VOLUME;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_NOISE_CLOCK;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AD(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_ADSR_ATTACK_RATE;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A4(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_PITCH_BEND_SLIDE;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A5(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_SET_OCTAVE;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_TRANSPOSE_ABSOLUTE;
    g_AkaoCmdArg0 = arg1 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_TRANSPOSE_RELATIVE;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_REVERB_ON;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg2 = arg3 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C8(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_LOOP_POINT;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C9(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_RETURN_LOOP_N_TIMES;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_CA(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_RETURN_LOOP;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    g_AkaoCmdArg2 = arg2;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D0(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_FULL_LENGTH_NOTE_ON;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D1(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_FULL_LENGTH_NOTE_OFF;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D2(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_FM_TOGGLE_DELAY_ON;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D4(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_PLAYBACK_RATE_SIDE_CHAIN_ON;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D5(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_PLAYBACK_RATE_SIDE_CHAIN_OFF;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D6(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_PITCH_VOLUME_SIDE_CHAIN_ON;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D8(int arg0) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_FINE_TUNE_ABSOLUTE;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D9(int arg0, int arg1) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_FINE_TUNE_RELATIVE;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_DA(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_PORTAMENTO_ON;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F0(void) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_JUMP_ON_REPEAT;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F1(void) {
    g_AkaoCmdOpcode = AKAO_SEQ_OP_BREAK_LOOP_ON_REPEAT;
    Akao_EnqueueStagedCommand();
}

int Spu_PollOrWaitSeqBusy(int arg0) {
    if (arg0 == 0) {
        while (D_8009D2E0 & 1) {
        }

        return 0;
    } else {
        return D_8009D2E0 & 1;
    }
}

void Spu_UploadSampleBlockBlocking(int arg0, int arg1) {
    while (Spu_UploadSampleBlock(arg0, arg1) == 1) {
    }
}

int Spu_GetTransferStatus(void) {
    return g_SpuTransferStatus;
}

void Spu_SetVoiceVolume(int arg0) {
    register unsigned int value asm("$2");

    if (g_AkaoPlaybackMode & 2) {
        value = arg0;
        value <<= 1;
        value += arg0;
        value <<= 2;
        value -= arg0;
        value += value << 5;
        value <<= 3;
        value -= arg0;
        value >>= 13;
        D_8009D1CB = value;
        D_8009D1C9 = value;
        D_8009D1CA = value;
        D_8009D1C8 = value;
    } else {
        value = arg0;
        D_8009D1CA = value;
        D_8009D1C8 = value;
        D_8009D1CB = 0;
        D_8009D1C9 = 0;
    }
    CdMix(&D_8009D1C8);
}

int Spu_SetStreamModeA(void) {
    g_AkaoStreamUploadMode = 1;
    return 0;
}

int Spu_WriteRegChecked(int arg0);
void Spu_UploadWithPrepare(int arg0, int arg1);
void Spu_RebaseStreamAddrs(unsigned char *ptr, int value, int count);

extern int D_8009D270;
extern int D_8009D2C8;
extern int D_8009CDE8;
extern int D_8009D2BC;
extern int D_8009D2E4;
extern int D_800B2900[];

int Akao_StepNoteSequencer(unsigned char *cursor, unsigned int upload_limit) {
    register unsigned char *cursor_reg asm("$16") = cursor;
    register unsigned int upload_limit_reg asm("$18") = upload_limit;
    register unsigned char *src asm("$20");
    register unsigned int total_size asm("$21");
    register unsigned int chunk_size asm("$17");
    int start;
    register int count asm("$19");
    register int spu_addr asm("$18");
    register int *dst asm("$4");
    register int base asm("$2");
    register int remaining asm("$2");
    int ret;
    int *state;

    if ((D_8009D270 & 1) != 0) {
        if (Spu_ValidateSampleHeader(cursor_reg) != 0) {
            return -1;
        }

        cursor_reg += 0x14;
        asm volatile("" : "=r"(cursor_reg) : "0"(cursor_reg));
        total_size = *(int *)cursor_reg;
        cursor_reg += 4;
        asm volatile("" : "=r"(cursor_reg) : "0"(cursor_reg));
        start = *(int *)cursor_reg;
        cursor_reg += 4;
        asm volatile("" : "=r"(cursor_reg) : "0"(cursor_reg));
        count = *(int *)cursor_reg;
        asm volatile("" : "=r"(cursor_reg) : "0"(cursor_reg));
        if (count != 0) {
            count -= start;
        } else {
            count = 0x100 - start;
        }

        cursor_reg += 0x24;
        asm volatile("" : "=r"(cursor_reg) : "0"(cursor_reg));
        src = cursor_reg;
        chunk_size = count << 6;
        cursor_reg += chunk_size;
        base = upload_limit_reg - 0x40;
        upload_limit_reg = base - chunk_size;
        asm volatile("" : "=r"(upload_limit_reg) : "0"(upload_limit_reg));
        chunk_size = upload_limit_reg;
        if (chunk_size >= total_size) {
            chunk_size = total_size;
        }

        spu_addr = 0x8000;
        if ((unsigned int)count < 0x31) {
            state = (int *)D_8009D2C8;
            if (state[0x1B] != 0) {
                if ((state[0x1A] & 0x100) == 0) {
                    spu_addr = 0x38000;
                    D_8009CDE8 |= 0x100;
                } else {
                    D_8009CDE8 &= -0x101;
                }
            } else if (state[1] != 0) {
                if ((state[0] & 0x100) == 0) {
                    spu_addr = 0x38000;
                    D_8009CDE8 |= 0x100;
                } else {
                    D_8009CDE8 &= -0x101;
                }
            } else {
                D_8009CDE8 &= -0x101;
            }
        } else {
            D_8009CDE8 &= -0x101;
        }

        Spu_WriteRegChecked(spu_addr);
        Spu_UploadWithPrepare((int)cursor_reg, chunk_size);
        D_8009D2BC = spu_addr + chunk_size;
        asm volatile("" ::: "memory");
        remaining = total_size - chunk_size;
        D_8009D2E4 = remaining;
        Spu_RebaseStreamAddrs(src, spu_addr, count);

        if (spu_addr == 0x8000) {
            dst = (int *)((char *)D_800B2900 + 0x800);
        } else {
            dst = (int *)((char *)D_800B2900 + 0x1400);
        }

        chunk_size = count << 4;
        do {
            chunk_size--;
            *dst++ = *(int *)src;
            src += 4;
        } while (chunk_size != 0);

        D_8009D270 &= -2;
        ret = D_8009D2E4;
    } else {
        Spu_WriteRegChecked(D_8009D2BC);
        chunk_size = upload_limit_reg;
        if (chunk_size >= D_8009D2E4) {
            chunk_size = D_8009D2E4;
        }

        Spu_UploadWithPrepare((int)cursor_reg, chunk_size);
        D_8009D2BC += chunk_size;
        D_8009D2E4 -= chunk_size;
        ret = D_8009D2E4;
    }

    return ret;
}

int Spu_SetStreamModeB(void) {
    g_AkaoStreamUploadMode = 2;
    return 0;
}

extern int D_800B4900[];
extern int D_800B4D00[];
extern int D_8009D1EC;
extern int D_8009D204;

int Spu_UploadStreamBlockA(int arg0, unsigned char *arg1, unsigned int arg2) {
    register int mode asm("$22") = arg0;
    register unsigned char *cursor asm("$16") = arg1;
    register unsigned int upload_limit asm("$18") = arg2;
    register unsigned char *src asm("$20");
    register unsigned int total_size asm("$21");
    register unsigned int chunk_size asm("$17");
    register int count asm("$19");
    register int spu_addr asm("$18");
    register int *dst asm("$4");
    register int ret asm("$2");
    register int start asm("$2");
    register int base asm("$3");
    register int remaining asm("$2");
    register int base_addr asm("$2");

    if ((D_8009D270 & 2) != 0) {
        if (Spu_ValidateSampleHeader(cursor) != 0) {
            return -1;
        }

        cursor += 0x14;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        total_size = *(int *)cursor;
        cursor += 4;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        start = *(int *)cursor;
        cursor += 4;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        count = *(int *)cursor;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        if (count != 0) {
            count -= start;
        } else {
            count = 0x100 - start;
        }

        cursor += 0x24;
        src = cursor;
        chunk_size = count << 6;
        cursor += chunk_size;
        start = upload_limit - 0x40;
        upload_limit = start - chunk_size;
        asm volatile("" : "=r"(upload_limit) : "0"(upload_limit));
        chunk_size = upload_limit;
        if (chunk_size >= total_size) {
            chunk_size = total_size;
        }

        base = 0x4F000;
        start = mode << 2;
        start += mode;
        start <<= 13;
        spu_addr = start + base;
        Spu_WriteRegChecked(spu_addr);
        Spu_UploadWithPrepare((int)cursor, chunk_size);
        {
            register unsigned char *src_arg asm("$4");
            register int spu_arg asm("$5");

            src_arg = src;
            spu_arg = spu_addr;
            remaining = spu_addr + chunk_size;
            D_8009D1EC = remaining;
            remaining = total_size - chunk_size;
            D_8009D204 = remaining;
            Spu_RebaseStreamAddrs(src_arg, spu_arg, count);
        }

        spu_addr = mode << 10;
        base_addr = (int)D_800B4D00;
        dst = (int *)(spu_addr + base_addr);
        chunk_size = count << 4;
        do {
            chunk_size--;
            *dst++ = *(int *)src;
            src += 4;
        } while (chunk_size != 0);

        D_8009D270 &= -3;
        ret = D_8009D204;
    } else {
        Spu_WriteRegChecked(D_8009D1EC);
        chunk_size = upload_limit;
        if (chunk_size >= D_8009D204) {
            chunk_size = D_8009D204;
        }

        Spu_UploadWithPrepare((int)cursor, chunk_size);
        D_8009D1EC += chunk_size;
        D_8009D204 -= chunk_size;
        ret = D_8009D204;
    }

    return ret;
}

int Spu_UploadStreamBlockB(int arg0, unsigned char *arg1) {
    register int mode asm("$21") = arg0;
    register unsigned char *cursor asm("$16") = arg1;
    register unsigned char *src asm("$17");
    register int size asm("$18");
    register int count asm("$19");
    register int spu_addr asm("$20");
    register int start asm("$2");
    register int *dst asm("$4");
    register int ret asm("$2");
    register int base asm("$3");
    register int base_addr asm("$2");

    if ((mode & -2) != 0) {
        return 1;
    }

    if (Spu_ValidateSampleHeader(cursor) != 0) {
        return -1;
    }

    cursor += 0x14;
    asm volatile("" : "=r"(cursor) : "0"(cursor));
    size = *(int *)cursor;
    cursor += 4;
    asm volatile("" : "=r"(cursor) : "0"(cursor));
    start = *(int *)cursor;
    cursor += 4;
    asm volatile("" : "=r"(cursor) : "0"(cursor));
    count = *(int *)cursor;
    asm volatile("" : "=r"(cursor) : "0"(cursor));
    if (count != 0) {
        count -= start;
    } else {
        count = 0x100 - start;
    }

    src = cursor + 0x24;
    start = count << 6;
    cursor = src + start;
    base = 0x68000;
    start = mode << 13;
    spu_addr = start + base;
    Spu_WriteRegChecked(spu_addr);
    Spu_UploadWithPrepare((int)cursor, size);
    Spu_RebaseStreamAddrs(src, spu_addr, count);

    spu_addr = mode << 10;
    base_addr = (int)D_800B4900;
    dst = (int *)(spu_addr + base_addr);
    size = count << 4;
    do {
        size--;
        *dst++ = *(int *)src;
        src += 4;
    } while (size != 0);

    asm volatile("addu %0, $0, $0" : "=r"(ret));
    return ret;
}

void Spu_WriteKeyOn(unsigned int value) {
    D_1F801D88 = value;
    value >>= 16;
    D_1F801D8A = value;
}

void Spu_WriteKeyOff(unsigned int value) {
    D_1F801D8C = value;
    value >>= 16;
    D_1F801D8E = value;
}

void Spu_WriteReverbEnable(unsigned int value) {
    D_1F801D98 = value;
    value >>= 16;
    D_1F801D9A = value;
}

void Spu_WriteNoiseEnable(unsigned int value) {
    D_1F801D94 = value;
    value >>= 16;
    D_1F801D96 = value;
}

void Spu_WriteFmEnable(unsigned int value) {
    D_1F801D90 = value;
    value >>= 16;
    D_1F801D92 = value;
}

void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right)
{
  unsigned int new_var;
  unsigned short *ptr = (unsigned short *) (0x1F801C00 + (((unsigned long long) index) * 0x10));
  new_var = 0x7FFF;
  ptr[0] = left & new_var;
  ptr[1] = right & 0x7FFF;
}

void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C04 / 2] = value;
}

void AkaoSpuVoice_SetStartAddress(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C06 / 2] = value >> 3;
}

void AkaoSpuVoice_SetRepeatAddress(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C0E / 2] = value >> 3;
}

void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *(unsigned char *)ptr;
    unsigned int value = ((right >> 2) << 15) | (left << 8);

    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrDecayRate(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *ptr;

    *ptr = (current & 0xFF0F) | (value << 4);
}

void AkaoSpuVoice_SetAdsrSustainLevel(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *ptr;

    *ptr = (current & 0xFFF0) | value;
}

void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C0A + index * 0x10);
    unsigned int value = ((right >> 1) << 14) | (left << 6);
    unsigned int current = *ptr;

    *ptr = (current & 0x3F) | value;
}

void AkaoSpuVoice_SetAdsrReleaseRate(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C0A + index * 0x10);
    unsigned int value = ((right >> 2) << 5) | left;
    unsigned int current = *ptr;

    *ptr = (current & 0xFFC0) | value;
}
