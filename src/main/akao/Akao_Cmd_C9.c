/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_C9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1;
    Akao_EnqueueStagedCommand();
}
