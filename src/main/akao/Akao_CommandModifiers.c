#include "pe1/akao/commands.h"

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

#include "pe1/akao/commands.h"

void Akao_Cmd_A8(int arg0) {
    g_AkaoCmdOpcode = 0xA8;
    g_AkaoCmdArg0 = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xA9;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A0(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA0;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0x7F;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA1;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_AA(int arg0) {
    g_AkaoCmdOpcode = 0xAA;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_AB(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xAB;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A2(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA2;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA3;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmdOpcode = 0xAC;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_AD(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xAD;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A4(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA4;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_A5(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA5;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC0;
    g_AkaoCmdArg0 = arg1 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xC1;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xC2;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg2 = arg3 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void func_80086CF8(int arg0) {
    g_AkaoCmdOpcode = 0xC8;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void func_80086D2C(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void func_80086D68(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xCA;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    g_AkaoCmdArg2 = arg2;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D0(int arg0) {
    g_AkaoCmdOpcode = 0xD0;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D1(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD1;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D2(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xD2;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D4(int arg0) {
    g_AkaoCmdOpcode = 0xD4;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D5(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD5;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D6(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xD6;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D8(int arg0) {
    g_AkaoCmdOpcode = 0xD8;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_D9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_DA(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xDA;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_F0(void) {
    g_AkaoCmdOpcode = 0xF0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_F1(void) {
    g_AkaoCmdOpcode = 0xF1;
    Akao_EnqueueStagedCommand();
}
