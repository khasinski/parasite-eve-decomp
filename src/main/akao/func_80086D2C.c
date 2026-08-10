#include "pe1/akao/commands.h"

void func_80086D2C(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}
