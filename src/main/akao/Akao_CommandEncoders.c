#include "pe1/akao/commands.h"

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
#include "pe1/akao/commands.h"

void Akao_Cmd_90(int arg0) {
    g_AkaoCmdOpcode = 0x90;
    g_AkaoCmdArg0 = arg0 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}
#include "pe1/akao/commands.h"

void Akao_Cmd_92(int arg0) {
    g_AkaoCmdOpcode = 0x92;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
