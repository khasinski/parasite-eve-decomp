/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xC0;
    g_AkaoCmdArg0 = arg1 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}
