#include "pe1/akao/commands.h"

void Akao_Cmd_F0(void) {
    g_AkaoCmdOpcode = 0xF0;
    Akao_EnqueueStagedCommand();
}
