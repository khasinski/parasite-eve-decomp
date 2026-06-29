/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_30(int arg0) {
    g_AkaoCmdOpcode = 0x30;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}
