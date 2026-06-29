#ifndef PE1_AKAO_COMMANDS_H
#define PE1_AKAO_COMMANDS_H

#include "pe1/akao/queue.h"

/*
 * Staging globals for the public AKAO command wrappers at 0x800864xx..0x800870xx.
 * The executor consumes these fields, allocates one or more AkaoQueueEntry
 * records, and returns a status value.
 */
extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;
extern int g_AkaoCmdArg2;
extern int g_AkaoCmdArg3;

int Akao_EnqueueStagedCommand(void);

#endif
