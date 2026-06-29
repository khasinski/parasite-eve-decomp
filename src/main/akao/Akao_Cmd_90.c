/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_90(int arg0) {
    g_AkaoCmdOpcode = 0x90;
    g_AkaoCmdArg0 = arg0 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}
