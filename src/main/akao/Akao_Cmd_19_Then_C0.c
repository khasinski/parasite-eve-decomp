/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    int *opcode = &g_AkaoCmdOpcode;
    int ret;

    *opcode = 0x19;
    g_AkaoCmdArg0 = arg0;
    ret = Akao_EnqueueStagedCommand();

    *opcode = 0xC0;
    g_AkaoCmdArg0 = arg1 & 0x7F;
    g_AkaoCmdArg3 = 0;
    Akao_EnqueueStagedCommand();

    return ret;
}
