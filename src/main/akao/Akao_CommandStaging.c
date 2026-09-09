#include "pe1/akao/commands.h"

void Akao_Cmd_10(int arg0) {
    g_AkaoCmdOpcode = 0x10;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_11(int arg0) {
    g_AkaoCmdOpcode = 0x11;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_40(void) {
    g_AkaoCmdOpcode = 0x40;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    register int *opcode asm("$17");
    register int saved_arg asm("$16");
    int next_opcode;
    int ret;

    opcode = &g_AkaoCmdOpcode;
    *opcode = 0x19;
    g_AkaoCmdArg0 = arg0;
    saved_arg = arg1;
    ret = Akao_EnqueueStagedCommand();

    next_opcode = 0xC0;
    asm volatile("" : : "r"(next_opcode));
    *opcode = next_opcode;
    saved_arg &= 0x7F;
    g_AkaoCmdArg0 = saved_arg;
    g_AkaoCmdArg3 = 0;
    saved_arg = ret;
    Akao_EnqueueStagedCommand();

    return saved_arg;
}

#include "pe1/akao/commands.h"

void Akao_Cmd_12(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0x12;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_20(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0x20;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

int Spu_ValidateSampleHeader(void);

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

#include "pe1/akao/commands.h"

void Akao_Cmd_21(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0x21;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_30(int arg0) {
    g_AkaoCmdOpcode = 0x30;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}
