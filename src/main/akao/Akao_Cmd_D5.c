/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_D5(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD5;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}
