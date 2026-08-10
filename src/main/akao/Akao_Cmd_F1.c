#include "pe1/akao/commands.h"

void Akao_Cmd_F1(void) {
    g_AkaoCmdOpcode = 0xF1;
    Akao_EnqueueStagedCommand();
}
