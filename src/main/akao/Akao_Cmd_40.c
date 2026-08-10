#include "pe1/akao/commands.h"

void Akao_Cmd_40(void) {
    g_AkaoCmdOpcode = 0x40;
    Akao_EnqueueStagedCommand();
}
