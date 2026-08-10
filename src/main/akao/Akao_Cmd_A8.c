#include "pe1/akao/commands.h"

void Akao_Cmd_A8(int arg0) {
    g_AkaoCmdOpcode = 0xA8;
    g_AkaoCmdArg0 = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}
