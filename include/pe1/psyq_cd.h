#ifndef PE1_PSYQ_CD_H
#define PE1_PSYQ_CD_H

#include "common.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_types.h"

typedef PsyqEventCallback CdlCB;

/* LIBCD's contiguous sync, ready and data-end interrupt event bytes. */
typedef struct CdInterruptEvents {
    volatile u8 sync;
    volatile u8 ready;
    volatile u8 end;
} CdInterruptEvents;

extern CdInterruptEvents D_8009B294;

/* Shared LIBCD hardware pointers: byte-wide indexed ports and word control. */
extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegPort1;
extern volatile u8 *g_CdRegDataWrite;
extern volatile u8 *g_CdRegResponse;
/* DMA code adds volatile through its access pointer; CD_flush writes once. */
extern u32 *g_CdRegRequest;

typedef union CdDmaInterruptRegister {
    volatile u32 word;
    volatile u8 bytes[4];
} CdDmaInterruptRegister;

extern CdDmaInterruptRegister *D_8009B348;
extern volatile u32 *D_8009B344;
extern volatile u8 *D_8009B32C;
extern char D_80011C2C[];

/* LIBCD keeps DMA register addresses in shared pointer slots. */
extern volatile u32 *D_8009B28C;
extern volatile u32 *D_8009B2B0, *D_8009B2B4;
extern void *volatile *D_8009B2B8;
extern volatile u32 *D_8009B2BC, *D_8009B2C0;
extern volatile u32 *g_CdRegDmaControl;

typedef void (*CdReadCompleteCallback)(int event, void *data);

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

PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, dataCallback) == 0x1C,
                  cd_read_progress_data_callback_offset);
PE1_STATIC_ASSERT(sizeof(CdReadProgressState) == 0x2C,
                  cd_read_progress_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, sectorSize) == 0x08,
                  cd_read_progress_sector_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, startVsync) == 0x20,
                  cd_read_progress_start_vsync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, currentVsync) == 0x24,
                  cd_read_progress_current_vsync_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdReadProgressState, inProgress) == 0x28,
                  cd_read_progress_in_progress_offset);

/* Optional 2048-byte-sector staging buffer used by the streaming CD path. */
extern u8 *D_800C0DB8;

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
extern StHEADER *StRingAddr;
extern u32 StRingSize;

/* C_004 streaming completion and back-location state. The byte-view alias
 * retains the retail unaligned CdlLOC copy; entries still use StHEADER fields.
 */
extern u8 *g_CdRingBufPtr;
extern int g_CdStreamRingReadSlot;
extern int g_CdStreamDataReadyFlag;
extern int D_800BE998;
extern CdlLOC D_800A3490;
extern int D_800A3494;
extern int D_800A8020;
extern volatile DsCallback g_StrDataReadyCallback;
int CdPosToInt_Local(CdlLOC *p);
CdlLOC *CdIntToPos_Local(int i, CdlLOC *p);
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

/* View beginning at D_8009B558, four bytes into the system-state window. */
typedef struct CdRomEventCommandState {
    u_char pendingCommand;
    u_char pendingParamBytes[4];
    u_char reserved05[3];
    u_char *pendingParams;
    u_char reserved0C[8];
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
PE1_STATIC_ASSERT(PE1_OFFSETOF(CdRomEventCommandState, pendingParams) == 0x08,
                  cdrom_event_pending_params_offset);

extern CdRomSystemState g_DsReadSysEnabled;
extern DsReadStatusBlock g_DsReadStatusBlock __asm__("D_8009B574");
extern CdRomCommandState g_CdSeekState;
extern CdRomEventCommandState g_CdRomEventCommandState
    __asm__("D_8009B558");
extern int g_CdRomCmdTimeout __asm__("D_8009B598");
extern int g_CdRomCmdRetryState __asm__("D_8009B59C");
extern int g_CdRomCmdLongTimeoutTable[];

/* This word is also the base address used by CdRom_DispatchPendingCmd. */
extern int g_CdDsReadIndex;

void CdRom_ReadProgressCallback(int status, void *data, void *detail);

extern CdReadProgressState g_CdReadProgress __asm__("D_8009B6A4");
#define g_CdReadStartVsync (g_CdReadProgress.startVsync)
#define g_CdReadCurrentVsync (g_CdReadProgress.currentVsync)
#define g_CdReadInProgress (g_CdReadProgress.inProgress)
extern CdReadCompleteCallback g_CdReadCompleteCallback;
CdReadCompleteCallback func_80081254(CdReadCompleteCallback callback);

void CdRom_AbortCmd(void);
void Cd_SetIntrMask(void);
/* BIOS_1.OBJ polling state, result buffers and timeout diagnostics. */
extern char *D_8009AFDC[], *D_8009B05C[];
extern volatile u8 *D_8009B27C, *D_8009B288;
extern u8 D_800A3460[8], D_800A3468[8], D_800A3470[8];
extern int D_800A3478, D_800A347C;
extern char *D_800A3480;
extern char D_80011B18[], D_80011B28[], D_80011BA0[], D_80011BA8[];
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
extern CdCommandTables D_8009B0FC;
extern int D_8009B1FC[];
extern int D_8009AFC0;
extern u8 D_8009AFD0[4];
extern u8 g_CdMode __asm__("D_8009AFD4");
extern u8 g_CdLastCom __asm__("D_8009AFD5");
extern volatile u8 *D_8009B280, *D_8009B284;
extern char D_80011BB4[], D_80011BBC[], D_80011BCC[];

/* Command/ready event polling; result is an optional eight-byte buffer. */
int CD_sync(int mode, u8 *result);
int CD_ready(int mode, u8 *result);
int CdSync(int mode, u8 *result);
int CdReady(int mode, u8 *result);
void CdRom_Sync(u8 *result);
void CdRom_SendReadyCommand(u8 *result);
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
int CdRom_DataSync(int mode);
int VSync(int mode);
int CdRom_IsBusy();
int CdRom_IsBusy2();
void Save_ProcessDataCallback(void);
DsCallback CdDataCallback(DsCallback callback);
void CdRom_SetMode2Callback(u_char event);
void Render_StepParticleCallback(void);

extern CdlLOC g_CdLastPos;
extern CdlLOC g_CdCurPosPtr;
extern int D_8009B260;
extern u32 g_CdStreamMask;
extern u32 D_800B6918;
extern u32 g_CdStreamEndSector;

/* CD streaming state shared by StSetStream, StSetRing and StCdInterrupt. */
extern s32 D_800A5D54;
extern s16 D_800A8018;
extern s32 D_800A801C;
extern u32 D_800C20C4;
typedef union CdStreamReadyState {
    s32 word;
    struct {
        s16 low;
        s16 high;
    } half;
} CdStreamReadyState;
PE1_STATIC_ASSERT(sizeof(CdStreamReadyState) == 4,
                  cd_stream_ready_state_size);
extern CdStreamReadyState D_800B0CD0;
/* The memcard overlay reads and clears only the low halfword at this address. */
extern s16 g_CdStreamReadyHalfword __asm__("D_800B0CD0");
extern volatile s32 D_800B6914, D_800B8620;
extern void (*D_800B0CCC)(void);
extern s32 g_CdStreamRingIndex __asm__("D_800BE9EC");
extern StHEADER *D_800C0DC8;

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
/* SDK CD_status occupies one word; public getters read its low byte. */
extern u32 D_8009AFC4;
extern u32 D_8009AFC8; /* SDK CD_status1, also stored as a full word. */
int CD_cw(int command, void *parameters, u8 *result, int mode);
void dma_execute(int channel, u32 address, int blockCount, int blockSize,
                 volatile u32 control, u8 interrupt, int reserved);
/* The streaming caller supplies a fourth word; the copy helper ignores it. */
void mem2mem(void *destination, void *source, unsigned int count, int reserved);
int CdControl(u_char command, u_char *parameters, u_char *result);
int CdControlF(u_char command, u_char *parameters);
int CdControlB(u_char command, u_char *parameters, u_char *result);
int CdMix(CdlATV *volume);
int CdGetSector(void *address, int size);
int CdGetSector2(void *address, int size);
DsCallback CdDataCallback(DsCallback callback);
int CdDataSync(int mode);

void CdRom_CmdEventCallback(int event, u8 *result);

#endif
