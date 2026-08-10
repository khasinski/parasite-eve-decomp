#include "pe1/akao/commands.h"

void Akao_Cmd_90(int arg0) {
    g_AkaoCmdOpcode = 0x90;
    g_AkaoCmdArg0 = arg0 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}
