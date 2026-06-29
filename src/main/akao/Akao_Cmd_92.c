/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_92(int arg0) {
    g_AkaoCmdOpcode = 0x92;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
