/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_CA(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xCA;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    g_AkaoCmdArg2 = arg2;
    Akao_EnqueueStagedCommand();
}
