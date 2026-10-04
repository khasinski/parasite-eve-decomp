#ifndef PE1_AKAO_QUEUE_H
#define PE1_AKAO_QUEUE_H

/*
 * AKAO message queue entry (0x24 bytes, the Square message shape that FF7's
 * AKAO queue also uses). Akao_ProcessMessageQueue dispatches on the low byte
 * of the opcode word; Akao_EnqueueStagedCommand writes the whole word.
 */
typedef struct AkaoQueueEntry {
    /* 0x00 */ union {
        unsigned char id;
        int word;
    } opcode;
    /* 0x04 */ union {
        int value;
        /* sample load messages: sample data past the header */
        unsigned short *sample_data;
    } arg0;
    /* 0x08 */ int arg1;
    /* 0x0C */ int arg2;
    /* 0x10 */ int arg3;
    /* 0x14 */ int sequence;
    /* 0x18 */ int unk_18;
    /* 0x1C */ int unk_1C;
    /* 0x20 */ int unk_20;
} AkaoQueueEntry;

typedef void (*AkaoMessageHandler)(AkaoQueueEntry *entry);

extern AkaoQueueEntry D_800B8628[];
extern int g_AkaoMessageQueueCount;
extern AkaoMessageHandler Akao_MessageHandlers[] asm("D_8009C0C0");

void Akao_MessageNoop(AkaoQueueEntry *entry);
void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg);
void Akao_ProcessMessageQueue(void);

#endif
