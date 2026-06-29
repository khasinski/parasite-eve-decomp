/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_11(int arg0) {
    g_AkaoCmdOpcode = 0x11;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
