/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"

/* Psy-Q DSFILE.OBJ: ISO-9660 media and directory cache helpers.
 * Byte arrays preserve unaligned, little-endian on-disc fields. */
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

PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoVolumePathTable, pathTableSectorLE) == 140,
                  iso_volume_path_table_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoPathRecord, name) == 8,
                  iso_path_name_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoDirectoryRecord, sizeLE) == 10,
                  iso_directory_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(IsoDirectoryRecord, name) == 33,
                  iso_directory_name_offset);

extern u8 D_800A52B0[2048];
extern int D_8009B6DC;
extern int D_8009AFC0;
int ds_read(int count, int sector, void *destination);
CdlLOC *CdIntToPos(int sector, CdlLOC *position);
void *memcpy(void *, const void *, unsigned int);
int strncmp(const char *, const char *, unsigned int);
int strcmp(const char *, const char *);
int printf(const char *, ...);
int puts(const char *);

extern const char D_80011EF8[], D_80011F24[], D_80011F2C[], D_80011F5C[];
extern const char D_80011F80[], D_80011FA0[], D_80011FB4[];
extern const char D_80011FD8[], D_80011FF8[], D_8001201C[], D_80012038[];
/* These two strings are word-aligned in dsfile.c, permitting halfword copies. */
extern const char D_80012014[] __attribute__((aligned(4)));
extern const char D_80012018[] __attribute__((aligned(4)));

/* Diagnostic columns traverse the minute, second and sector bytes with
 * the DslFILE stride. All three views share g_DslFileCache storage. */
typedef struct DsStridedFileByte {
    u8 value;
    char remainder[sizeof(DslFILE) - sizeof(u8)];
} DsStridedFileByte;
extern DsStridedFileByte file_minutes[DSL_MAX_FILE] __asm__("D_800A36B0");
extern DsStridedFileByte file_seconds[DSL_MAX_FILE] __asm__("D_800A36B1");
extern DsStridedFileByte file_sectors[DSL_MAX_FILE] __asm__("D_800A36B2");

int DS_newmedia(void) {
    u8 *cursor;
    u8 *end;
    char *name;
    int sector;
    int count;
    int read_status;
    IsoPathRecord *record;
    u8 *limit;
    char *sectors;
    char *names;

    read_status = ds_read(1, 16, D_800A52B0);
    if (read_status != 1) {
        if (D_8009AFC0 > 0) puts(D_80011EF8);
        return 0;
    }
    if (strncmp(((IsoVolumePathTable *)D_800A52B0)->identifier,
                D_80011F24, 5) != 0) {
        if (D_8009AFC0 > 0) puts(D_80011F2C);
        return 0;
    }
    end = ((IsoVolumePathTable *)D_800A52B0)->pathTableSectorLE;
    memcpy(&sector, end, 4);
    if (ds_read(1, sector, D_800A52B0) != read_status) {
        if (D_8009AFC0 > 0) printf(D_80011F5C, sector);
        return 0;
    }
    if (D_8009AFC0 > 1) puts(D_80011F80);
    cursor = D_800A52B0;
    end = cursor + 2048;
    count = 0;
    if (cursor >= end) goto finished;
    sectors = (char *)&g_DslDirectoryCache[0].sector;
    names = sectors + 4;
    limit = end;
next_record:
    {
        record = (IsoPathRecord *)cursor;
        if (record->nameLength == 0) goto finished;
        memcpy(count * sizeof(DslDirectoryCacheEntry) + sectors,
               record->sectorLE, 4);
        name = (char *)((u32)(count * sizeof(DslDirectoryCacheEntry)) +
                       (u32)names);
        g_DslDirectoryCache[count].parentDirectoryId =
            record->parentDirectoryLE[0];
        g_DslDirectoryCache[count].directoryId = count + 1;
        memcpy(name, record->name, record->nameLength);
        name[record->nameLength] = 0;
        cursor += record->nameLength + 8 + (record->nameLength & 1);
        if (D_8009AFC0 > 1)
            printf(D_80011FA0, g_DslDirectoryCache[count].sector,
                   g_DslDirectoryCache[count].directoryId,
                   g_DslDirectoryCache[count].parentDirectoryId, name);
        if (++count >= DSL_MAX_DIR) goto finished;
        if (cursor < limit) goto next_record;
    }
finished:
    if (count < DSL_MAX_DIR) g_DslDirectoryCache[count].parentDirectoryId = 0;
    D_8009B6DC = 0;
    if (D_8009AFC0 > 1) printf(D_80011FB4, count);
    return 1;
}

/* Psy-Q DSFILE.OBJ: find a directory by its parent ID and name. */
int DS_searchdir(int parent, char *name) {
    int i;

    for (i = 0; i < DSL_MAX_DIR; i++) {
        if (!g_DslDirectoryCache[i].parentDirectoryId) {
            break;
        }
        if (g_DslDirectoryCache[i].parentDirectoryId != parent) {
            continue;
        }
        if (strcmp(name, g_DslDirectoryCache[i].name) == 0) {
            return i + 1;
        }
    }
    return -1;
}

/* Psy-Q DSFILE.OBJ: cache one ISO directory and its file positions. */
int DS_cachefile(int directory) {
    IsoDirectoryRecord *cursor;
    int sector;
    int count;

    if (directory == D_8009B6DC) return 1;
    if (ds_read(1, (g_DslDirectoryCache + directory)[-1].sector,
                D_800A52B0) != 1) {
        if (D_8009AFC0 > 0) puts(D_80011FD8);
        return -1;
    }
    if (D_8009AFC0 > 1) puts(D_80011FF8);
    cursor = (IsoDirectoryRecord *)D_800A52B0;
    for (count = 0; (u8 *)cursor < D_800A52B0 + 2048;) {
        if (cursor->recordLength == 0) break;
        memcpy(&sector, cursor->sectorLE, 4);
        CdIntToPos(sector, &g_DslFileCache[count].pos);
        memcpy(&g_DslFileCache[count].size, cursor->sizeLE, 4);
        switch (count) {
        case 0:
            memcpy(g_DslFileCache[0].name, D_80012014, 2);
            break;
        case 1:
            memcpy(g_DslFileCache[1].name, D_80012018, 3);
            break;
        default:
            memcpy(g_DslFileCache[count].name, cursor->name, cursor->nameLength);
            g_DslFileCache[count].name[cursor->nameLength] = 0;
        }
        if (D_8009AFC0 > 1)
            printf(D_8001201C, file_minutes[count].value,
                   file_seconds[count].value, file_sectors[count].value,
                   g_DslFileCache[count].size, g_DslFileCache[count].name);
        cursor = (IsoDirectoryRecord *)((u8 *)cursor + cursor->recordLength);
        if (++count >= DSL_MAX_FILE) break;
    }
    D_8009B6DC = directory;
    if (count < DSL_MAX_FILE) g_DslFileCache[count].name[0] = 0;
    if (D_8009AFC0 > 1) printf(D_80012038, count);
    return 1;
}
