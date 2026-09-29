#ifndef PE1_PSYQ_DS_H
#define PE1_PSYQ_DS_H

#include "pe1/psyq_cd.h"

/* Psy-Q LIBDS.H file record returned by DsSearchFile. */
typedef struct DslFILE {
    CdlLOC pos;
    u_int size;
    char name[16];
} DslFILE;

/* DS_newmedia fills this table from ISO-9660 path-table records. */
typedef struct DslDirectoryCacheEntry {
    int directoryId;
    int parentDirectoryId;
    u_int sector;
    char name[32];
} DslDirectoryCacheEntry;

enum {
    DSL_MAX_FILE = 64,
    DSL_MAX_DIR = 128,
};

extern DslFILE g_DslFileCache[DSL_MAX_FILE] __asm__("D_800A36B0");
extern DslDirectoryCacheEntry g_DslDirectoryCache[DSL_MAX_DIR]
    __asm__("D_800A3CB0");

typedef union DsCallbackValue {
    DsCallback poll;
    DsEventCallback event;
    u32 word;
} DsCallbackValue;

PE1_STATIC_ASSERT(sizeof(DsCallbackValue) == 4, ds_callback_value_size);

/* Runtime callback pointers at D_800A36A0 are a contiguous four-word window. */
typedef struct DsRuntimeCallbacks {
    DsCallback volatile poll;
    DsEventCallback volatile sync;
    DsEventCallback volatile ready;
    DsEventCallback volatile dispatch;
} DsRuntimeCallbacks;

PE1_STATIC_ASSERT(sizeof(DsRuntimeCallbacks) == 16,
                  ds_runtime_callbacks_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsRuntimeCallbacks, sync) == 4,
                  ds_runtime_sync_callback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsRuntimeCallbacks, ready) == 8,
                  ds_runtime_ready_callback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsRuntimeCallbacks, dispatch) == 12,
                  ds_runtime_dispatch_callback_offset);

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
extern DsEventCallback volatile g_DsSyncCallback __asm__("D_800A36A4");
extern DsEventCallback volatile g_DsReadyCallback __asm__("D_800A36A8");
extern DsCallback volatile g_DsPollCallback __asm__("g_DsPollCallback");
extern DsEventCallback volatile g_DsDispatchCallback __asm__("D_800A36AC");
extern CdDsReadQueueEntry g_CdDsReadQueue[];
extern int g_CdDsReadQueueState;
extern int g_CdPendingReadCount;

PE1_STATIC_ASSERT(sizeof(DslFILE) == 0x18, dsl_file_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, size) == 0x04,
                  dsl_file_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, name) == 0x08,
                  dsl_file_name_offset);
PE1_STATIC_ASSERT(sizeof(DslDirectoryCacheEntry) == 0x2C,
                  dsl_directory_cache_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, directoryId) ==
                      0x00,
                  dsl_directory_cache_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, parentDirectoryId) ==
                      0x04,
                  dsl_directory_cache_parent_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, sector) == 0x08,
                  dsl_directory_cache_sector_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslDirectoryCacheEntry, name) == 0x0C,
                  dsl_directory_cache_name_offset);

/* ISO-9660 structures used by Psy-Q LIBDS/DSFILE.OBJ. Disk integers are byte
 * arrays because several fields are unaligned and little-endian. */
typedef struct IsoVolumePathTable {
    u8 type;
    char identifier[5];
    u8 version;
    u8 reserved07[133];
    u8 pathTableSectorLE[4];
} IsoVolumePathTable;

typedef struct IsoPathRecord {
    u8 nameLength;
    u8 extendedAttributeLength;
    u8 sectorLE[4];
    u8 parentDirectoryLE[2];
    char name[1];
} IsoPathRecord;

typedef struct IsoDirectoryRecord {
    u8 recordLength;
    u8 extendedAttributeLength;
    u8 sectorLE[4];
    u8 sectorBE[4];
    u8 sizeLE[4];
    u8 sizeBE[4];
    u8 timestamp[7];
    u8 flags;
    u8 fileUnitSize;
    u8 interleaveGapSize;
    u8 volumeSequenceLE[2];
    u8 volumeSequenceBE[2];
    u8 nameLength;
    char name[1];
} IsoDirectoryRecord;

extern u8 g_DsFileSectorBuffer[2048] __asm__("D_800A52B0");
extern int g_DsCachedDirectory __asm__("D_8009B6DC");
extern int g_DsCachedDiskType __asm__("D_8009B6E0");

typedef void (*DsAsyncReadCallback)(int status, void *data, void *detail);

typedef struct DsAsyncReadState {
    /* g_DsReadBusy names the final active field at offset 0x20. */
    int nextSector;
    int lastDeliveredSector;
    DsAsyncReadCallback callback;
    int retryPending;
    int retriesRemaining;
    DsEventCallback savedSyncCallback;
    DsEventCallback savedReadyCallback;
    int reserved1C;
    int active;
} DsAsyncReadState;

int CdRom_InitAsyncRead(DsAsyncReadCallback callback, int callbackArg);

PE1_STATIC_ASSERT(sizeof(DsAsyncReadState) == 0x24, ds_async_read_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, retryPending) == 0x0C,
                  ds_async_read_retry_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, retriesRemaining) == 0x10,
                  ds_async_read_retries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, savedSyncCallback) == 0x14,
                  ds_async_read_saved_sync_callback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, savedReadyCallback) == 0x18,
                  ds_async_read_saved_ready_callback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, active) == 0x20,
                  ds_async_read_active_offset);

/* LIBCD callback page contains LIBDS async-read state in its reserved span. */
typedef struct CdReadCompleteCallbackPage {
    CdReadCompleteCallback callback;
    char reserved04[0x18];
    DsAsyncReadState asyncRead;
    char reserved40[0x48F0];
} CdReadCompleteCallbackPage;

typedef struct CdCallbackDataPage {
    CdlCB sync;
    CdlCB ready;
    int reserved08;
    CdlCB read;
    int status;
    char reserved14[0x6DC];
    CdReadProgressState readProgress;
    CdReadCompleteCallbackPage readComplete;
} CdCallbackDataPage;

PE1_STATIC_ASSERT(sizeof(CdReadCompleteCallbackPage) == 0x4930,
                  cd_read_callback_page_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadCompleteCallbackPage, asyncRead) == 0x1C,
                  cd_read_callback_async_state_offset);
PE1_STATIC_ASSERT(sizeof(CdCallbackDataPage) == 0x504C,
                  cd_callback_data_page_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, sync) == 0x00,
                  cd_callback_data_page_sync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, ready) == 0x04,
                  cd_callback_data_page_ready_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, read) == 0x0C,
                  cd_callback_data_page_read_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, status) == 0x10,
                  cd_callback_data_page_status_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, readProgress) == 0x6F0,
                  cd_callback_data_page_read_progress_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, readComplete) == 0x71C,
                  cd_callback_data_page_read_complete_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCallbackDataPage, readComplete) +
                      PE1_OFFSETOF(CdReadCompleteCallbackPage, asyncRead) ==
                      0x738,
                  cd_callback_data_page_async_state_offset);

extern DsAsyncReadState g_DsAsyncReadState __asm__("D_8009B6EC");
#define g_DsAsyncReadRetryPending (g_DsAsyncReadState.retryPending)
#define g_DsAsyncReadSavedSyncCallback \
    (g_DsAsyncReadState.savedSyncCallback)
#define g_DsAsyncReadSavedReadyCallback \
    (g_DsAsyncReadState.savedReadyCallback)
extern int g_DsReadBusy;
#define DS_ASYNC_READ_STATE_FROM_ACTIVE(active_pointer) \
    ((DsAsyncReadState *)((char *)(active_pointer) - \
                          PE1_OFFSETOF(DsAsyncReadState, active)))
#define DS_ASYNC_READ_FIELD(active_pointer, field) \
    (DS_ASYNC_READ_STATE_FROM_ACTIVE(active_pointer)->field)

PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoVolumePathTable, pathTableSectorLE) == 140,
                  iso_volume_path_table_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoPathRecord, name) == 8,
                  iso_path_name_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoDirectoryRecord, sizeLE) == 10,
                  iso_directory_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoDirectoryRecord, name) == 33,
                  iso_directory_name_offset);

int ds_read(int count, int sector, void *destination);
int DS_newmedia(void);
int DS_searchdir(int parent, char *name);
int DS_cachefile(int directory);
void DsReadBreak(void);
int _cmp(char *left, char *right);
DslFILE *DsSearchFile(DslFILE *file, char *name);
int DsGetDiskType(void);
int strncmp(const char *left, const char *right, unsigned int count);
int strcmp(const char *left, const char *right);

#endif /* PE1_PSYQ_DS_H */
