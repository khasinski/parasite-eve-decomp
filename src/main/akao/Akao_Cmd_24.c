/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

int Spu_ValidateSampleHeader(void);

void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3) {
    if (Spu_ValidateSampleHeader() != 0) {
        return;
    }

    g_AkaoCmdOpcode = 0x24;
    g_AkaoCmdArg0 = arg0 + 4;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    g_AkaoCmdArg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}
