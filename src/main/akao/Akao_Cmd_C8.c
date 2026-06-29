/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_C8(int arg0) {
    g_AkaoCmdOpcode = 0xC8;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
