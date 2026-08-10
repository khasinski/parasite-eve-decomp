#include "pe1/akao/commands.h"

void Akao_Cmd_D1(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD1;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}
