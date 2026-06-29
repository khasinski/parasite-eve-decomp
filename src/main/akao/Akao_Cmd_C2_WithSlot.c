/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xC2;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg2 = arg3 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}
