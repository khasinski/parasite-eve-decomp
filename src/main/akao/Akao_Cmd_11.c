#include "pe1/akao/commands.h"

void Akao_Cmd_11(int arg0) {
    g_AkaoCmdOpcode = 0x11;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
