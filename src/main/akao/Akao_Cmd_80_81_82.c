/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"


void Akao_Cmd_80_81_82(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x81;
        break;
    case 2:
        opcode = 0x82;
        break;
    default:
        opcode = 0x80;
        break;
    }

    g_AkaoCmdOpcode = opcode;
    Akao_EnqueueStagedCommand();
}
