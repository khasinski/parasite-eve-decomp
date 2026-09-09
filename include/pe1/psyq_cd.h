#ifndef PE1_PSYQ_CD_H
#define PE1_PSYQ_CD_H

#include "common.h"
#include "pe1/psyq_callbacks.h"

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef void (*CdlCB)(u_char event, u_char *result);
typedef void (*DsCallback)(void);
typedef void (*DsEventCallback)(u_char event, u_char *result);

/* LIBCD's contiguous sync, ready and data-end interrupt event bytes. */
typedef struct CdInterruptEvents {
    volatile u8 sync;
    volatile u8 ready;
    volatile u8 end;
} CdInterruptEvents;

extern CdInterruptEvents D_8009B294;

typedef struct CdCallbackDataPage {
    char reserved00[4];
    CdlCB sync;
    CdlCB ready;
    int reserved0C;
    CdlCB read;
    int status;
    char reserved18[0x5038];
} CdCallbackDataPage;

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

/* Psy-Q LIBDS.H's DslFILE, returned by DsSearchFile. */
typedef struct DslFILE {
    CdlLOC pos;
    u_int size;
    char name[16];
} DslFILE;

/* LIBDS.H defines DslMAXDIR as 128.  DS_newmedia fills this cache from
 * ISO-9660 directory records and DS_searchdir traverses it by these fields. */
typedef struct DslDirectoryCacheEntry {
    int parentDirectoryId;
    int recordNameLength;
    u_int sector;
    char name[32];
} DslDirectoryCacheEntry;

enum {
    DSL_MAX_FILE = 64,
    DSL_MAX_DIR = 128,
};

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
    u_char pendingParamBytes[4];
    u_char reserved05[3];
    u_char *pendingParams;
    u_char reserved0C[8];
    CdRomCommandState command;
} CdRomEventCommandState;

PE1_STATIC_ASSERT(sizeof(DsReadStatusBlock) == 0x28,
                  ds_read_status_block_size);
PE1_STATIC_ASSERT(sizeof(DslFILE) == 0x18, dsl_file_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, size) == 0x04,
                  dsl_file_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, name) == 0x08,
                  dsl_file_name_offset);
PE1_STATIC_ASSERT(sizeof(DslDirectoryCacheEntry) == 0x2C,
                  dsl_directory_cache_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, parentDirectoryId) ==
                      0x00,
                  dsl_directory_cache_parent_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, recordNameLength) ==
                      0x04,
                  dsl_directory_cache_name_length_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, sector) == 0x08,
                  dsl_directory_cache_sector_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, name) == 0x0C,
                  dsl_directory_cache_name_offset);
PE1_STATIC_ASSERT(sizeof(CdCallbackDataPage) == 0x5050,
                  cd_callback_data_page_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, sync) == 0x04,
                  cd_callback_data_page_sync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, ready) == 0x08,
                  cd_callback_data_page_ready_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, read) == 0x10,
                  cd_callback_data_page_read_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, status) == 0x14,
                  cd_callback_data_page_status_offset);
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
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, pendingParams) == 0x08,
                  cdrom_event_pending_params_offset);

extern CdRomSystemState g_DsReadSysEnabled;
extern CdRomCommandState g_CdSeekState;
extern CdRomEventCommandState g_CdRomEventCommandState
    __asm__("D_8009B558");
extern int g_CdRomCmdTimeout __asm__("D_8009B598");
extern int g_CdRomCmdRetryState __asm__("D_8009B59C");
extern int g_CdRomCmdLongTimeoutTable[];

/* libds/dsfile.c cache tables. */
extern DslFILE g_DslFileCache[DSL_MAX_FILE] __asm__("D_800A36B0");
extern DslDirectoryCacheEntry g_DslDirectoryCache[DSL_MAX_DIR]
    __asm__("D_800A3CB0");

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

int Render_AllocParticleNode(int command, void *parameter, int arg2, int arg3);
int DsControlF(u_char command, u_char *parameter);

typedef struct CdDsReadQueueEntry {
    u_int active;
    u_char command;
    u_char payload[4];
    u_char unk_09[3];
    void *parameter;
    u_int arg10;
    u_int arg14;
} CdDsReadQueueEntry;

/* Contiguous queue storage and bookkeeping, anchored by the pending count. */
typedef struct CdDsReadQueueWindow {
    CdDsReadQueueEntry entries[8];
    int queue_state;
    int read_index;
    int pending_count;
} CdDsReadQueueWindow;
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdDsReadQueueWindow, pending_count) == 0xC8,
                  ds_queue_pending_count_offset);
#define CD_DS_QUEUE_FROM_PENDING(pointer) \
    ((CdDsReadQueueWindow *)((char *)(pointer) - \
                            PE1_OFFSETOF(CdDsReadQueueWindow, pending_count)))

typedef struct DsReadCallbackSlot {
    int value;
    u_char command;
    u_char payload[8];
    u_char reserved0D[3];
} DsReadCallbackSlot;

PE1_STATIC_ASSERT(sizeof(DsReadCallbackSlot) == 0x10,
                  ds_read_callback_slot_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadCallbackSlot, command) == 0x04,
                  ds_read_callback_slot_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsReadCallbackSlot, payload) == 0x05,
                  ds_read_callback_slot_payload_offset);

typedef void (*DsAsyncReadCallback)(int status, void *data, void *detail);
int CdRom_InitAsyncRead(DsAsyncReadCallback callback, int callbackArg);
void CdRom_ReadProgressCallback(int status, void *data, void *detail);

typedef struct DsAsyncReadState {
    /* g_DsReadBusy names the final active field at offset 0x20. */
    int result;
    int reserved04;
    DsAsyncReadCallback callback;
    int reserved0C;
    int callback_arg;
    DsEventCallback saved_sync_callback;
    DsEventCallback saved_ready_callback;
    int reserved1C;
    int active;
} DsAsyncReadState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, active) == 0x20,
                  ds_async_read_active_offset);

extern int g_DsReadBusy;
#define DS_ASYNC_READ_STATE_FROM_ACTIVE(active_pointer) \
    ((DsAsyncReadState *)((char *)(active_pointer) - \
                          PE1_OFFSETOF(DsAsyncReadState, active)))
#define DS_ASYNC_READ_FIELD(active_pointer, field) \
    (DS_ASYNC_READ_STATE_FROM_ACTIVE(active_pointer)->field)

typedef struct CdCallbackDataWindow {
    CdlCB syncCallback;
    u_char pad_AFB8[0x5048];
} CdCallbackDataWindow;

PE1_STATIC_ASSERT(sizeof(CdCallbackDataWindow) == 0x504C,
                  cd_callback_data_window_size);

typedef struct CdReadProgressState {
    int reserved00[2];
    int sectorSize;
    int destination;
    int remainingSectors;
    int flags;
    int eventData;
    DsCallback dataCallback;
    int startVsync;
    int currentVsync;
    int inProgress;
} CdReadProgressState;

typedef void (*CdReadCompleteCallback)(int event, int data);

PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, dataCallback) == 0x1C,
                  cd_read_progress_data_callback_offset);
PE1_STATIC_ASSERT(sizeof(CdReadProgressState) == 0x2C,
                  cd_read_progress_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, sectorSize) == 0x08,
                  cd_read_progress_sector_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, currentVsync) == 0x24,
                  cd_read_progress_current_vsync_offset);

extern CdDsReadQueueEntry g_CdDsReadQueue[];
extern int g_CdDsReadIndex;
extern int g_CdDsReadQueueState;
extern int g_CdPendingReadCount;
typedef struct DsCallbackRegistry {
    int start; /* The start callback ABI is not recovered yet. */
    DsEventCallback sync;
    DsEventCallback ready;
} DsCallbackRegistry;
PE1_STATIC_ASSERT(sizeof(DsCallbackRegistry) == 12, ds_callback_registry_size);
extern DsCallbackRegistry g_DsReadCallbackState __asm__("D_800B8AB0");
extern CdQueuedCmdSlot g_CdQueuedCmdSlots[3] __asm__("D_800A3510");
extern DsReadCallbackSlot g_DsReadCallbackSlots[8] __asm__("D_800A3610");
extern int g_DsReadCallbackCursor __asm__("D_800A3690");
extern CdReadProgressState g_CdReadProgress __asm__("D_8009B6A4");
extern CdReadCompleteCallback g_CdReadCompleteCallback;

void CdRom_AbortCmd(void);
DslFILE *DsSearchFile(DslFILE *file, char *name);
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
int Render_PlayParticleEffect(int command, u8 *payload);
void CdRom_DispatchDsReadyCallback(void);
void CdRom_PollPendingDsRead(void);
/* Data DMA synchronization: mode 0 waits; nonzero polls. */
int CD_datasync(int mode);
int CdDataSync(int mode);
int CdRom_DataSync(int mode);
int VSync(int mode);
int CdRom_IsBusy();
int CdRom_IsBusy2();
void Save_ProcessDataCallback(void);
int DsRead_IsBusy(void);
int DsGetDiskType(void);
extern volatile int g_DsDiskType;
CdlLOC *CdRom_GetCurrentPos(CdlLOC *destination);
int Render_BuildParticleFrame();
DsEventCallback DsSyncCallback(DsEventCallback callback);
DsEventCallback DsReadyCallback(DsEventCallback callback);
DsCallback DsDataCallback(DsCallback callback);
DsCallback CdDataCallback(DsCallback callback);
void CdRom_SetMode2Callback(u_char event);
void Render_StepParticleCallback(void);

extern CdlLOC g_CdLastPos;
extern int D_8009B260;
extern int g_CdStreamMask;
extern int D_800B6918;
extern int g_CdStreamEndSector;

/* Low-level LIBCD command retry wrapper and its shared command state. */
extern u32 D_8009AF2C[];
extern CdlCB D_8009AFB4;
extern CdlCB D_8009AFB8;
extern CdlCB g_CdSyncCallback;
extern CdlCB g_CdReadyCallback;
extern CdlCB g_CdReadCallback;
CdlCB CdReadCallback(CdlCB callback);
CdlCB CdSyncCallback(CdlCB callback);
CdlCB CdReadyCallback(CdlCB callback);
CdlCB CdRom_SetReadCallback(CdlCB callback);
extern u8 D_8009AFC4;
int CD_cw(int command, void *parameters, u8 *result, int mode);
int func_8007A4D0(int command, void *parameters, u8 *result);

#endif
