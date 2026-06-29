/* CC1_PROFILE: build0001 */
#include "pe1/akao.h"

void Akao_Cmd_F1(void) {
    g_AkaoCmdOpcode = 0xF1;
    Akao_EnqueueStagedCommand();
}
