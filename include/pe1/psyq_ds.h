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

typedef struct DsAsyncReadState {
    /* g_DsReadBusy names the final active field at offset 0x20. */
    int nextSector;
    int lastDeliveredSector;
    DsAsyncReadCallback callback;
    int retryPending;
    int retriesRemaining;
    DsEventCallback saved_sync_callback;
    DsEventCallback saved_ready_callback;
    int reserved1C;
    int active;
} DsAsyncReadState;

PE1_STATIC_ASSERT(sizeof(DsAsyncReadState) == 0x24, ds_async_read_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, retryPending) == 0x0C,
                  ds_async_read_retry_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, retriesRemaining) == 0x10,
                  ds_async_read_retries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DsAsyncReadState, active) == 0x20,
                  ds_async_read_active_offset);

extern DsAsyncReadState g_DsAsyncReadState __asm__("D_8009B6EC");
extern int g_DsAsyncReadRetryPending __asm__("D_8009B6F8");
extern DsEventCallback g_DsAsyncReadSavedSyncCallback __asm__("D_8009B700");
extern DsEventCallback g_DsAsyncReadSavedReadyCallback __asm__("D_8009B704");
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
