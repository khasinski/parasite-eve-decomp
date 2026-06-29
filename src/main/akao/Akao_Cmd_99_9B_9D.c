/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"


void Akao_Cmd_99_9B_9D(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9B;
        break;
    case 2:
        opcode = 0x9D;
        break;
    default:
        opcode = 0x99;
        break;
    }

    g_AkaoCmdOpcode = opcode;
    Akao_EnqueueStagedCommand();
}
