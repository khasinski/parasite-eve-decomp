/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_F0(void) {
    g_AkaoCmdOpcode = 0xF0;
    Akao_EnqueueStagedCommand();
}
