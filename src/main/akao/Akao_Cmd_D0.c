/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_D0(int arg0) {
    g_AkaoCmdOpcode = 0xD0;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}
