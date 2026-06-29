/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xA1;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}
