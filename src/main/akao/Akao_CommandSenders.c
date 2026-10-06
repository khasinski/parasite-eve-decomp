#include "pe1/akao/commands.h"

void Akao_Cmd_10(int arg0) {
    g_AkaoCmd.opcode = 0x10;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_11(int arg0) {
    g_AkaoCmd.opcode = 0x11;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_40(void) {
    g_AkaoCmd.opcode = 0x40;
    Akao_EnqueueStagedCommand();
}

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    int *opcode;
    int saved_arg;
    int next_opcode;
    int ret;

    opcode = &g_AkaoCmd.opcode;
    *opcode = 0x19;
    g_AkaoCmd.arg0.value = arg0;
    saved_arg = arg1;
    ret = Akao_EnqueueStagedCommand();

    next_opcode = 0xC0;
    *opcode = next_opcode;
    saved_arg &= 0x7F;
    g_AkaoCmd.arg0.value = saved_arg;
    g_AkaoCmd.arg3 = 0;
    saved_arg = ret;
    Akao_EnqueueStagedCommand();

    return saved_arg;
}

void Akao_Cmd_12(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0x12;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_20(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0x20;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

int Spu_ValidateSampleHeader(void);

void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3) {
    if (Spu_ValidateSampleHeader() != 0) {
        return;
    }

    g_AkaoCmd.opcode = 0x24;
    g_AkaoCmd.arg0.value = arg0 + 4;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_21(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0x21;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_30(int arg0) {
    g_AkaoCmd.opcode = 0x30;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
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

    g_AkaoCmd.opcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_90(int arg0) {
    g_AkaoCmd.opcode = 0x90;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_92(int arg0) {
    g_AkaoCmd.opcode = 0x92;
    g_AkaoCmd.arg0.value = arg0;
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

    g_AkaoCmd.opcode = opcode;
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

    g_AkaoCmd.opcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A8(int arg0) {
    g_AkaoCmd.opcode = 0xA8;
    g_AkaoCmd.arg0.value = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xA9;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    g_AkaoCmd.arg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A0(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xA0;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xA1;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AA(int arg0) {
    g_AkaoCmd.opcode = 0xAA;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AB(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xAB;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A2(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xA2;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xA3;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmd.opcode = 0xAC;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AD(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xAD;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A4(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xA4;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A5(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xA5;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xC0;
    g_AkaoCmd.arg0.value = arg1 & 0x7F;
    g_AkaoCmd.arg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xC1;
    g_AkaoCmd.arg0.value = arg1;
    g_AkaoCmd.arg1 = arg2 & 0x7F;
    g_AkaoCmd.arg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xC2;
    g_AkaoCmd.arg0.value = arg1;
    g_AkaoCmd.arg1 = arg2 & 0x7F;
    g_AkaoCmd.arg2 = arg3 & 0x7F;
    g_AkaoCmd.arg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void func_80086CF8(int arg0) {
    g_AkaoCmd.opcode = 0xC8;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

void func_80086D2C(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xC9;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void func_80086D68(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xCA;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    g_AkaoCmd.arg2 = arg2;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D0(int arg0) {
    g_AkaoCmd.opcode = 0xD0;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D1(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xD1;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D2(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xD2;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D4(int arg0) {
    g_AkaoCmd.opcode = 0xD4;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D5(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xD5;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D6(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xD6;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D8(int arg0) {
    g_AkaoCmd.opcode = 0xD8;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D9(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xD9;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_DA(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xDA;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F0(void) {
    g_AkaoCmd.opcode = 0xF0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F1(void) {
    g_AkaoCmd.opcode = 0xF1;
    Akao_EnqueueStagedCommand();
}
