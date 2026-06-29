#ifndef PE1_AKAO_QUEUE_H
#define PE1_AKAO_QUEUE_H

#include "pe1/battle_cmd.h"

typedef void (*AkaoMessageHandler)(AkaoQueueEntry *entry);

extern AkaoQueueEntry D_800B8628[];
extern int g_AkaoMessageQueueCount;
extern AkaoMessageHandler Akao_MessageHandlers[] asm("D_8009C0C0");

void Akao_MessageNoop(AkaoQueueEntry *entry);
void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg);
void Akao_ProcessMessageQueue(void);

#endif
