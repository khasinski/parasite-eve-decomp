#ifndef PE1_PSYQ_CD_H
#define PE1_PSYQ_CD_H

#include "common.h"

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef void (*DsCallback)(void);

typedef struct CdlLOC {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

typedef struct CdlATV {
    u_char val0;
    u_char val1;
    u_char val2;
    u_char val3;
} CdlATV;

typedef struct DsDecodedEventFlags {
    u_char bit7;
    u_char bit6;
    u_char bit5;
    u_char bit1;
} DsDecodedEventFlags;

typedef struct DsReadStatusBlock {
    u_int status;
    u_int command;
    u_int sector;
    u_char lastCommand;
    u_char commandMode;
    CdlLOC currentPos;
    u_char retryCount;
    u_char commandParam;
    DsDecodedEventFlags eventFlags;
    u_int reserved18;
    u_int discType;
    u_int syncResult;
    u_int readyResult;
} DsReadStatusBlock;

typedef struct CdRomCommandState {
    u_char eventStatus;
    u_char reserved01[3];
    u_int eventValue;
    DsReadStatusBlock read;
    u_int retryAttempts;
    u_int reserved34;
} CdRomCommandState;

typedef struct CdRomSystemState {
    u_int enabled;
    u_char pendingCommand;
    u_char pendingMode;
    u_char reserved06[6];
    u_char preSeekState[0x0C];
    CdRomCommandState command;
} CdRomSystemState;

/* View beginning at D_8009B558, four bytes into CdRomSystemState. */
typedef struct CdRomEventCommandState {
    u_char pendingCommand;
    u_char pendingMode;
    u_char reserved02[0x12];
    CdRomCommandState command;
} CdRomEventCommandState;

PE1_STATIC_ASSERT(sizeof(DsReadStatusBlock) == 0x28,
                  ds_read_status_block_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadStatusBlock, lastCommand) == 0x0C,
                  ds_read_status_last_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadStatusBlock, currentPos) == 0x0E,
                  ds_read_status_current_pos_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadStatusBlock, retryCount) == 0x12,
                  ds_read_status_retry_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadStatusBlock, eventFlags) == 0x14,
                  ds_read_status_event_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadStatusBlock, discType) == 0x1C,
                  ds_read_status_disc_type_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadStatusBlock, readyResult) == 0x24,
                  ds_read_status_ready_result_offset);
PE1_STATIC_ASSERT(sizeof(CdRomCommandState) == 0x38,
                  cdrom_command_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomCommandState, read) == 0x08,
                  cdrom_command_read_offset);
PE1_STATIC_ASSERT(sizeof(CdRomSystemState) == 0x50,
                  cdrom_system_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemState, pendingCommand) == 0x04,
                  cdrom_system_pending_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemState, command) == 0x18,
                  cdrom_system_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, command) == 0x14,
                  cdrom_event_command_offset);

extern CdRomSystemState g_DsReadSysEnabled;
extern CdRomCommandState g_CdSeekState;

typedef struct DsReadyEventWindow {
    u_char eventStatus;
    u_char unk_01[7];
    u_int status;
    u_int command;
} DsReadyEventWindow;

typedef struct CdQueuedCmdSlot {
    u_int state;
    u_char result;
    u_char payload[8];
    u_char unk_0D[3];
} CdQueuedCmdSlot;

typedef struct CdDsReadQueueEntry {
    u_int active;
    u_char command;
    u_char payload[4];
    u_char unk_09[3];
    u_int arg0C;
    u_int arg10;
    u_int arg14;
} CdDsReadQueueEntry;

typedef struct DsReadCallbackSlot {
    int value;
    int reserved[3];
} DsReadCallbackSlot;

extern CdDsReadQueueEntry g_CdDsReadQueue[];
extern int g_CdDsReadIndex;
extern int g_CdDsReadQueueState;
extern int g_CdPendingReadCount;
extern int g_DsReadCallbackState[3] __asm__("D_800B8AB0");
extern CdQueuedCmdSlot g_CdQueuedCmdSlots[3] __asm__("D_800A3510");
extern DsReadCallbackSlot g_DsReadCallbackSlots[8] __asm__("D_800A3610");
extern int g_DsReadCallbackCursor __asm__("D_800A3690");

void CdRom_AbortCmd(void);
void CQ_clear_queue(void *queue);
void DS_read_cbready(void);
DsCallback DsReadCallback(DsCallback callback);
void CdRom_EnableDsReadSystem(void);
int CdRom_IsDsReadSystemEnabled(void);
void CdRom_InitDsCallbacks(void);
void CdRom_SetSyncCallback(unsigned int value);
void CdRom_SetReadyCallback(unsigned int value);
void CdRom_SetDispatchCallback(unsigned int value);
void CdRom_SetPollCallback(unsigned int value);
void Render_StepParticleNode(void);
void Render_PlayParticleEffect(void);
void CdRom_DispatchDsReadyCallback(void);
void CdRom_PollPendingDsRead(void);

#endif
