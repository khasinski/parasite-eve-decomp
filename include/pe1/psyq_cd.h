#ifndef PE1_PSYQ_CD_H
#define PE1_PSYQ_CD_H

#include "common.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_types.h"

typedef PsyqEventCallback CdlCB;

/* LIBCD's contiguous sync, ready and data-end interrupt event bytes. */
typedef struct CdInterruptEvents {
    volatile u8 sync;
    volatile u8 ready;
    volatile u8 end;
} CdInterruptEvents;

PE1_STATIC_ASSERT(sizeof(CdInterruptEvents) == 3, cd_interrupt_events_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdInterruptEvents, sync) == 0,
                  cd_interrupt_events_sync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdInterruptEvents, ready) == 1,
                  cd_interrupt_events_ready_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdInterruptEvents, end) == 2,
                  cd_interrupt_events_end_offset);

extern CdInterruptEvents g_CdInterruptEvents;

/* Shared LIBCD hardware pointers: byte-wide indexed ports and word control. */
extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegPort1;
extern volatile u8 *g_CdRegDataWrite;
extern volatile u8 *g_CdRegResponse;
/* The common delay register (0x1F801020); LIBCD writes 0x1325 or 0x1323. */
extern volatile u32 *g_CdRegRequest;
extern u16 *volatile g_CdSpuRegBase;

typedef union CdDmaInterruptRegister {
    volatile u32 word;
    volatile u8 bytes[4];
    struct {
        u8 reserved00[2];
        /* dma_execute changes one channel's enable bit in this byte. */
        volatile u8 enableControl;
        u8 reserved03;
    } fields;
} CdDmaInterruptRegister;
typedef char CdDmaInterruptRegisterSize[
    (sizeof(CdDmaInterruptRegister) == 4) ? 1 : -1];

extern CdDmaInterruptRegister *g_CdDmaInterruptRegPtr;
extern volatile u32 *g_CdDmaControlRegPtr;
extern volatile u8 *g_CdStIntrIndexRegPtr;
extern char g_CdDmaStatusErrorFormat[];

/* LIBCD keeps DMA register addresses in shared pointer slots. */
extern volatile u32 *g_CdRegDelaySize, *g_CdRegDmaPcr;
extern void *volatile *g_CdRegDmaMadr;
extern volatile u32 *g_CdRegDmaBcr, *g_CdRegDmaControl;

typedef void (*CdReadCompleteCallback)(int event, void *data);

typedef struct CdReadProgressState {
    s32 commandPollToggle;
    s32 reserved04;
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

PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, dataCallback) == 0x1C,
                  cd_read_progress_data_callback_offset);
PE1_STATIC_ASSERT(sizeof(CdReadProgressState) == 0x2C,
                  cd_read_progress_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, commandPollToggle) == 0,
                  cd_read_progress_command_poll_toggle_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, reserved04) == 4,
                  cd_read_progress_reserved04_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, sectorSize) == 0x08,
                  cd_read_progress_sector_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, startVsync) == 0x20,
                  cd_read_progress_start_vsync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, currentVsync) == 0x24,
                  cd_read_progress_current_vsync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, inProgress) == 0x28,
                  cd_read_progress_in_progress_offset);

/* Optional 2048-byte-sector staging buffer used by the streaming CD path. */
extern u8 *g_CdStreamMemorySource;

typedef struct CdlLOC {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

/* Psy-Q LIBCD.H streaming-sector header. Each ring entry is 32 bytes. */
typedef struct StHEADER {
    u_short id;
    u_short type;
    u_short secCount;
    u_short nSectors;
    u32 frameCount;
    u32 frameSize;
    u_short width;
    u_short height;
    u32 dummy1;
    u32 dummy2;
    CdlLOC loc;
} StHEADER;

PE1_STATIC_ASSERT(sizeof(StHEADER) == 0x20, st_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(StHEADER, frameCount) == 0x08,
                  st_header_frame_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(StHEADER, loc) == 0x1C,
                  st_header_location_offset);

/* LIBCD streaming ring: StRingSize headers followed by sector payloads. */
/* Each ring slot's sector payload: 2048 bytes less its 32-byte header. */
typedef struct StSectorPayload {
    u32 words[0x1F8];
} StSectorPayload;

extern StHEADER *StRingAddr;
/* A ring header addressed by its byte offset. StFreeRing and init_ring_status
 * clear its leading status as a plain halfword or word, not a member. */
typedef union StHeaderAddress {
    StHEADER *header;
    s16 *status;
    s32 *statusWord;
    u32 word;
} StHeaderAddress;
extern u32 StRingSize;

/* C_004 streaming completion and back-location state. The byte-view alias
 * retains the retail unaligned CdlLOC copy; entries still use StHEADER fields.
 */
extern u8 *g_CdRingBufPtr;
extern int g_CdStreamRingReadSlot;
extern int g_CdStreamDataReadyFlag;
extern int g_CdStreamRingWriteSlot;
extern CdlLOC g_StLastFrameLoc;
extern int g_StLastFrameNumber;
extern int g_DsStreamNoLocFlag;
extern volatile DsCallback g_StrDataReadyCallback;
int CdPosToInt(CdlLOC *p);
CdlLOC *CdIntToPos(int i, CdlLOC *p);
void data_ready_callback(void);
int StGetBackloc(CdlLOC *position);

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

/* View beginning at g_CdRomEventCommandState (0x8009B558), four bytes into
 * the system-state window. */
typedef struct CdRomEventCommandState {
    u_char pendingCommand;
    u_char pendingParamBytes[4];
    u_char reserved05[3];
    u_char *pendingParams;
    u_char eventResult[8];
    CdRomCommandState command;
} CdRomEventCommandState;

typedef struct CdRomSystemFields {
    u_char pendingCommand;
    u_char pendingMode;
    u_char reserved06[6];
    u_char preSeekState[0x0C];
    CdRomCommandState command;
} CdRomSystemFields;

typedef union CdRomSystemViews {
    CdRomSystemFields system;
    CdRomEventCommandState event;
} CdRomSystemViews;

typedef struct CdRomSystemState {
    u_int enabled;
    CdRomSystemViews view;
} CdRomSystemState;

#define CDROM_SYSTEM_COMMAND_OFFSET \
    (PE1_OFFSETOF(CdRomSystemState, view) + \
     PE1_OFFSETOF(CdRomSystemViews, system) + \
     PE1_OFFSETOF(CdRomSystemFields, command))
#define CDROM_SYSTEM_READ_COMMAND_OFFSET \
    (CDROM_SYSTEM_COMMAND_OFFSET + \
     PE1_OFFSETOF(CdRomCommandState, read) + \
     PE1_OFFSETOF(DsReadStatusBlock, command))

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
PE1_STATIC_ASSERT(sizeof(CdRomSystemFields) == 0x4C,
                  cdrom_system_fields_size);
PE1_STATIC_ASSERT(sizeof(CdRomEventCommandState) == 0x4C,
                  cdrom_event_command_state_size);
PE1_STATIC_ASSERT(sizeof(CdRomSystemViews) == 0x4C,
                  cdrom_system_views_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemFields, command) == 0x14,
                  cdrom_system_fields_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemViews, event) == 0,
                  cdrom_system_views_event_offset);
PE1_STATIC_ASSERT(sizeof(CdRomSystemState) == 0x50,
                  cdrom_system_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemState, view) +
                      PE1_OFFSETOF(CdRomSystemViews, event) == 0x04,
                  cdrom_system_event_view_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemState, view) +
                      PE1_OFFSETOF(CdRomSystemViews, system) +
                      PE1_OFFSETOF(CdRomSystemFields, pendingCommand) == 0x04,
                  cdrom_system_pending_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomSystemState, view) +
                      PE1_OFFSETOF(CdRomSystemViews, system) +
                      PE1_OFFSETOF(CdRomSystemFields, command) == 0x18,
                  cdrom_system_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, command) == 0x14,
                  cdrom_event_command_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, eventResult) == 0x0C,
                  cdrom_event_result_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, pendingParams) == 0x08,
                  cdrom_event_pending_params_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, command) +
                      PE1_OFFSETOF(CdRomCommandState, eventValue) == 0x18,
                  cdrom_event_value_address_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, command) +
                      PE1_OFFSETOF(CdRomCommandState, read) +
                      PE1_OFFSETOF(DsReadStatusBlock, syncResult) == 0x3C,
                  cdrom_sync_result_countdown_address_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, command) +
                      PE1_OFFSETOF(CdRomCommandState, read) +
                      PE1_OFFSETOF(DsReadStatusBlock, readyResult) == 0x40,
                  cdrom_ready_result_timeout_address_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, command) +
                      PE1_OFFSETOF(CdRomCommandState, retryAttempts) == 0x44,
                  cdrom_retry_attempts_address_offset);

extern CdRomSystemState g_DsReadSysEnabled;
extern DsReadStatusBlock g_DsReadStatusBlock;
extern CdRomCommandState g_CdSeekState;
extern CdRomEventCommandState g_CdRomEventCommandState;
/* Direct word aliases of the overlapping command-state structure fields. */
extern s32 g_CdRomCommandEventValue;
extern s32 g_DsSyncResultCountdown;
extern int g_CdRomCmdTimeout;
extern int g_CdRomCmdRetryState;
extern int g_CdRomCmdLongTimeoutTable[];
/* A long-timeout flag addressed by the command's word offset. */
typedef union CdTimeoutFlagAddress {
    int *flag;
    u32 word;
} CdTimeoutFlagAddress;

/* Recover the shared command window from its timeout/ready-result word. */
#define CDROM_EVENT_STATE_FROM_TIMEOUT(pointer) \
    ((CdRomEventCommandState *)((char *)(pointer) - \
        PE1_OFFSETOF(CdRomEventCommandState, command.read.readyResult)))
/* The same window addressed from its retry counter. */
#define CDROM_EVENT_STATE_FROM_RETRY(pointer) \
    ((CdRomEventCommandState *)((char *)(pointer) - \
        PE1_OFFSETOF(CdRomEventCommandState, command.retryAttempts)))

/* This word is also the base address used by CQ_execute. */
extern int g_CdDsReadIndex;

void DS_read_cbready(int status, void *data, void *detail);

extern CdReadProgressState g_CdReadProgress;
/* Scalar view of the first field: LIBDS_DSSYS_1_text_4A4 reads it as a plain
 * word; through the structure GCC keeps its address live in $s0. */
extern s32 g_CdReadCommandPollToggle __asm__("g_CdReadProgress");
#define g_CdReadStartVsync (g_CdReadProgress.startVsync)
#define g_CdReadCurrentVsync (g_CdReadProgress.currentVsync)
#define g_CdReadInProgress (g_CdReadProgress.inProgress)
extern CdReadCompleteCallback g_CdReadCompleteCallback;
CdReadCompleteCallback DsReadCallback(CdReadCompleteCallback callback);

void DS_stop(void);
void callback(void);
/* BIOS_1.OBJ polling state, result buffers and timeout diagnostics. */
extern const char *g_CdCommandNames[], *g_CdInterruptNames[];
extern u32 g_CdCommandAckOnly[];
extern u8 g_CdSyncResult[8], g_CdReadyResult[8], g_CdEndResult[8];
extern int g_CdTimeoutDeadlineVsync, g_CdTimeoutPollCount;
extern char *g_CdTimeoutFuncName;
extern char g_CdTimeoutText[], g_CdTimeoutDetailFormat[], g_CdSyncName[], g_CdReadyName[];
int getintr(void);
void CD_flush(void);

/* BIOS_1 command tables, three adjacent 32-command arrays. The middle
 * table controls status updates on interrupt 3 (see getintr). */
typedef struct CdCommandTables {
    int ready_flags[32];
    int update_status_on_ack[32];
    int parameter_counts[32];
} CdCommandTables;
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdCommandTables, parameter_counts) == 0x100,
                  cd_command_parameter_counts_offset);
PE1_STATIC_ASSERT(sizeof(CdCommandTables) == 0x180, cd_command_tables_size);
extern CdCommandTables g_CdCommandTables;
extern u8 g_CdMode;
extern u8 g_CdLastCom;
extern char g_CdCommandTraceFormat[], g_CdNoParamFormat[], g_CdCwName[];
/* "$Id: bios.c,v 1.86 ...$", the LIBCD BIOS version string. */
extern const char g_CdBiosVersion[];

/* Command/ready event polling; result is an optional eight-byte buffer. */
int CD_sync(int mode, u8 *result);
int CD_ready(int mode, u8 *result);
int CdSync(int mode, u8 *result);
int CdReady(int mode, u8 *result);
void DS_sync(u8 *result);
void DS_ready(u8 *result);
/* Data DMA synchronization: mode 0 waits; nonzero polls. */
int CD_datasync(int mode);
int CdDataSync(int mode);
int CdStatus(void);
int CdMode(void);
int CdLastCom(void);
void StClearRing(void);
void init_ring_status(int start, u32 count);
void StSetMask(u32 mask, u32 start, u32 end);
void StSetStream(u32 mode, u32 startFrame, u32 endFrame,
                 void (*callback1)(void), void (*callback2)(void));
void StUnSetRing(void);
u32 StFreeRing(u32 *base);
u32 StGetNext(u32 **addr, u32 **header);
void StCdInterrupt(void);
int DsDataSync(int mode);
int DsGetSector();
int DsGetSector2();
void DsReadBreak(void);
DsCallback CdDataCallback(DsCallback callback);
void DS_read_cbsync(u_char event);
void DS_read_cbdata(void);

extern CdlLOC g_CdLastPos;

/* LIBCD BIOS descriptor: CD_init prints its address. */
typedef struct CdBiosInitDescriptor {
    CdInterruptEvents *events;
    u8 *sync_result;
    u8 *command;
    u32 *status;
    CdlLOC *position;
    const char *version;
} CdBiosInitDescriptor;
extern CdBiosInitDescriptor g_CdBiosInitDescriptor;
extern CdlLOC g_CdCurPosPtr;
extern u32 g_CdStreamMask;
extern u32 g_CdStreamStartFrame;

/* CD streaming state shared by StSetStream, StSetRing and StCdInterrupt. */
extern s32 g_StCurrentFrameNumber;
extern s16 g_StFrameSectorIndex;
extern s32 g_StRgb24Mode;
/* Contiguous end-of-stream controls. StCdInterrupt retains its two interior
 * scalar aliases because field-based addressing changes its retail schedule. */
typedef struct CdStreamEndState {
    u32 endSector;
    s32 endSectorPending;
    u8 *dmaBuffer;
} CdStreamEndState;
PE1_STATIC_ASSERT(sizeof(CdStreamEndState) == 0x0C,
                  cd_stream_end_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdStreamEndState, endSectorPending) == 0x04,
                  cd_stream_end_pending_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdStreamEndState, dmaBuffer) == 0x08,
                  cd_stream_dma_buffer_offset);
extern CdStreamEndState g_CdStreamEndState;
typedef union CdStreamReadyState {
    s32 word;
    struct {
        s16 low;
        s16 high;
    } half;
} CdStreamReadyState;
PE1_STATIC_ASSERT(sizeof(CdStreamReadyState) == 4,
                  cd_stream_ready_state_size);
extern CdStreamReadyState g_CdStreamReady;
/* The memcard overlay reads and clears only the low halfword, as a scalar:
 * through the union its test and clear schedule differently. */
extern s16 g_CdStreamReadyHalfword __asm__("g_CdStreamReady");
extern volatile s32 D_800B6914, g_CdStreamExpectedTypeId;
extern volatile s32 D_800B63D4;
extern void (*g_StrEndCallback)(void);
extern s32 g_CdStreamRingIndex;

/* Low-level LIBCD command retry wrapper and its shared command state. */
extern u32 g_CdCommandHasParam[];
extern CdlCB g_CdSyncCallback;
extern CdlCB g_CdReadyCallback;
extern CdlCB g_CdReadCallback;
extern int CD_debug;
int CdSetDebug(int level);
CdlCB CdSyncCallback(CdlCB callback);
CdlCB CdReadyCallback(CdlCB callback);
int DsSetDebug(int level);
/* SDK CD_status occupies one word; public getters read its low byte. */
extern u32 g_CdStatus;
extern u32 g_CdStatus1; /* SDK CD_status1, also stored as a full word. */
/* Counts status words that newly report the shell (lid) open. */
extern int g_CdLidOpenCount;
int CD_cw(int command, void *parameters, u8 *result, int mode);
void dma_execute(int channel, void *address, int blockCount, int blockSize,
                 volatile u32 control, u8 interrupt, int reserved);
/* The streaming caller supplies a fourth word; the copy helper ignores it. */
void mem2mem(void *destination, void *source, unsigned int count, int reserved);
int CdControl(u_char command, u_char *parameters, u_char *result);
int CdControlF(u_char command, u_char *parameters);
int CdControlB(u_char command, u_char *parameters, u_char *result);
int CdMix(CdlATV *volume);
int DsMix(CdlATV *volume);
int CD_vol(CdlATV *volume);
int CD_getsector(void *destination, int words);
int CD_getsector2(void *destination, int words);
int CdGetSector2(void *address, int size);
int CdGetSector(void *address, int size);

void LIBDS_DSSYS_1_text_8B8(int event, u8 *result);
void LIBDS_DSSYS_1_text_A9C(int event, u8 *result);
void LIBDS_DSSYS_1_text_D24(int event, u8 *result);

#endif
