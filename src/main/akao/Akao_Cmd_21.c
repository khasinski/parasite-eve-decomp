/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_21(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0x21;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}
