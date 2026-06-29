/* CC1_PROFILE: build0001 */

#include "pe1/akao.h"

void Akao_ProcessMessageQueue(void) {
    AkaoQueueEntry *entry = D_800B8628;

    if (g_AkaoMessageQueueCount != 0) {
        do {
            Akao_MessageHandlers[entry->opcode](entry);
            g_AkaoMessageQueueCount--;
            entry++;
        } while (g_AkaoMessageQueueCount != 0);
    }
}
