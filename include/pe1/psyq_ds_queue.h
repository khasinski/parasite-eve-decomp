#ifndef PE1_PSYQ_DS_QUEUE_H
#define PE1_PSYQ_DS_QUEUE_H

#include "pe1/psyq_cd.h"

/* DSSYS_2.OBJ's stack packet, distinct from its 24-byte pending queue entry.
 * The callback word is forwarded unchanged; this code never calls it. */
typedef struct DsPacketCommand {
    u8 command;
    CdlLOC payload;
    u8 reserved05[3];
    void *parameter;
    u32 callback;
} DsPacketCommand;

/* History and the published sync result share all four words, including pad. */
typedef union DsResult {
    DsReadCallbackSlot slot;
    int words[4];
} DsResult;

PE1_STATIC_ASSERT(sizeof(DsPacketCommand) == 16, ds_packet_command_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsPacketCommand, payload) == 1,
                  ds_packet_payload_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsPacketCommand, parameter) == 8,
                  ds_packet_parameter_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsPacketCommand, callback) == 12,
                  ds_packet_callback_offset);
PE1_STATIC_ASSERT(sizeof(DsResult) == 16, ds_result_size);

/* DSSYS_2.OBJ collects one callback per consecutive command identifier
 * before clearing the queue and notifying its clients. */
typedef struct DsQueueCallback {
    u32 id;
    DsEventCallback callback;
} DsQueueCallback;
typedef struct DsQueueIndices {
    int start, current, count;
} DsQueueIndices;
PE1_STATIC_ASSERT(sizeof(DsQueueCallback) == 8, ds_queue_callback_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsQueueCallback, callback) == 4,
                  ds_queue_callback_pointer_offset);
PE1_STATIC_ASSERT(sizeof(DsQueueIndices) == 12, ds_queue_indices_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsQueueIndices, count) == 8, ds_queue_count_offset);
extern DsQueueIndices D_800A3600;
void CdRom_EnqueueCmd(u32 id, u8 event, u8 *result);

extern int D_8009B4BC[];
extern u32 D_8009B53C;
extern int D_800A3608, D_800A3604;
extern CdDsReadQueueEntry D_800A3540[];
CdDsReadQueueEntry *Spu_GetQueueEntryPtr(void);
void Util_Copy4(void *, const void *);
void Util_Copy8(void *, const void *);
/* Legacy queue-state helper name; SDK DsSync is Render_FindParticleEffect. */
int DsSync(int mode);
void CdRom_DispatchPendingCmd(void);
int CdPosToInt(CdlLOC *);
int Render_FindParticleEffect(int id, void *result);

#endif
