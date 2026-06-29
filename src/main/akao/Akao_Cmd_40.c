/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_40(void) {
    g_AkaoCmdOpcode = 0x40;
    Akao_EnqueueStagedCommand();
}
