#include "include_asm.h"

/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

int Spu_ValidateSampleHeader(void);

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

    *opcode = 0xC0;
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
    g_AkaoCmdOpcode = 0xA8;
    g_AkaoCmdArg0 = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xA9;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A0(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA0;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA1;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AA(int arg0) {
    g_AkaoCmdOpcode = 0xAA;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AB(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xAB;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A2(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA2;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA3;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmdOpcode = 0xAC;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AD(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xAD;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A4(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA4;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A5(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA5;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC0;
    g_AkaoCmdArg0 = arg1 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xC1;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xC2;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg2 = arg3 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C8(int arg0) {
    g_AkaoCmdOpcode = 0xC8;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_CA(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xCA;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    g_AkaoCmdArg2 = arg2;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D0(int arg0) {
    g_AkaoCmdOpcode = 0xD0;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D1(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD1;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D2(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xD2;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D4(int arg0) {
    g_AkaoCmdOpcode = 0xD4;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D5(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD5;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D6(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xD6;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D8(int arg0) {
    g_AkaoCmdOpcode = 0xD8;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_DA(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xDA;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F0(void) {
    g_AkaoCmdOpcode = 0xF0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F1(void) {
    g_AkaoCmdOpcode = 0xF1;
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

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/tu_076C64", Akao_StepNoteSequencer);

int Spu_SetStreamModeB(void) {
    g_AkaoStreamUploadMode = 2;
    return 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/tu_076C64", Spu_UploadStreamBlockA);

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/tu_076C64", Spu_UploadStreamBlockB);

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
