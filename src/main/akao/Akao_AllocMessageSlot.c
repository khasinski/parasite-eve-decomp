#include "pe1/battle_cmd.h"

extern AkaoQueueEntry D_800B8628[];
extern int g_AkaoMessageQueueCount;

void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg) {
    *out_msg = D_800B8628;
    *out_msg = &D_800B8628[g_AkaoMessageQueueCount];
    g_AkaoMessageQueueCount++;
}
