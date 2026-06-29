/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"


void Akao_Cmd_98_9A_9C(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9A;
        break;
    case 2:
        opcode = 0x9C;
        break;
    default:
        opcode = 0x98;
        break;
    }

    g_AkaoCmdOpcode = opcode;
    Akao_EnqueueStagedCommand();
}
