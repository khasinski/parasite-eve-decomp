/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xA9;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}
