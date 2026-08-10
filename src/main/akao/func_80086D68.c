#include "pe1/akao/commands.h"

void func_80086D68(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xCA;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    g_AkaoCmdArg2 = arg2;
    Akao_EnqueueStagedCommand();
}
