#include "pe1/akao/commands.h"

void Akao_Cmd_92(int arg0) {
    g_AkaoCmdOpcode = 0x92;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
