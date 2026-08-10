#include "pe1/akao/commands.h"

void Akao_Cmd_12(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0x12;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}
