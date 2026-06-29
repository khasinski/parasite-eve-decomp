/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA3;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}
