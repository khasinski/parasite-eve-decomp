#include "pe1/akao/commands.h"

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmdOpcode = 0xAC;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}
