#include "pe1/akao/commands.h"

void Akao_Cmd_10(int arg0) {
    g_AkaoCmd.opcode = 0x10;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_11(int arg0) {
    g_AkaoCmd.opcode = 0x11;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_40(void) {
    g_AkaoCmd.opcode = 0x40;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

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

#include "pe1/akao/commands.h"

void Akao_Cmd_12(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0x12;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_20(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0x20;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

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

#include "pe1/akao/commands.h"

void Akao_Cmd_21(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0x21;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

#include "pe1/akao/commands.h"

void Akao_Cmd_30(int arg0) {
    g_AkaoCmd.opcode = 0x30;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}
