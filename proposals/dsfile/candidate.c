/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

/* Byte fields retain the unaligned, little-endian disk representation. */
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
extern char D_80011EF8[], D_80011F24[], D_80011F2C[], D_80011F5C[];
extern char D_80011F80[], D_80011FA0[], D_80011FB4[], D_80011FD8[];
extern char D_80011FF8[], D_8001201C[], D_80012038[];
extern char D_80012014[2] __attribute__((aligned(2)));
extern char D_80012018[3] __attribute__((aligned(2)));

static int ds_read(int count, int sector, int destination);
CdlLOC *CdIntToPos(int sector, CdlLOC *position);
void *memcpy(void *destination, const void *source, unsigned int size);
int strncmp(const char *left, const char *right, unsigned int size);
int strcmp(const char *left, const char *right);
int printf(const char *format, ...);
int puts(const char *text);

extern int D_8009B6E0;
extern char D_80011E6C[], D_80011E88[], D_80011EA0[], D_80011EBC[], D_80011EDC[], D_80011EE8[];
int CdRom_GetDiskType(void);
int CdRom_StartRead(CdlLOC *position, int count, int destination, int mode);
int Sys_VSyncTimeout(int argument);
static int DS_newmedia(void);
static int DS_searchdir(int parent, char *name);
static int DS_cachefile(int directory);
static int _cmp(char *left, char *right);

DslFILE *DsSearchFile(DslFILE *out, char *name) {
    char component[32];
    signed char first_character;
    signed char *path;
    signed char *component_start;
    char *cursor;
    int depth, directory;
    u_int not_found;
    DslFILE *entry;
    if (D_8009B6E0 < CdRom_GetDiskType()) {
        if (!DS_newmedia()) return 0;
        D_8009B6E0 = CdRom_GetDiskType();
    }
    first_character = *(signed char *)name;
    component_start = (signed char *)component;
    if (first_character != '\\') return 0;
    component[0] = 0;
    directory = 1;
    path = (signed char *)name;
    for (depth = 0; depth < 8; depth++) {
        cursor = component;
        while (*path != '\\' && *path) *cursor++ = *path++;
        if (!*path) break;
        path++;
        not_found = -1;
        *cursor = 0;
        directory = DS_searchdir(directory, component);
        if (directory == not_found) { component[0] = 0; break; }
    }
    if (depth >= 8) {
        if (D_8009AFC0 > 0) printf(D_80011E6C, name, depth);
        return 0;
    }
    if (!*component_start) {
        if (D_8009AFC0 > 0) printf(D_80011E88, name);
        return 0;
    }
    *cursor = 0;
    if (!DS_cachefile(directory)) {
        if (D_8009AFC0 > 0) puts(D_80011EA0);
        return 0;
    }
    if (D_8009AFC0 > 1) printf(D_80011EBC, component);
    for (depth = 0, entry = g_DslFileCache; depth < 64; depth++, entry++) {
        if (!*(signed char *)g_DslFileCache[depth].name) break;
        if (_cmp(entry->name, component)) {
            if (D_8009AFC0 > 1) printf(D_80011EDC, component);
            *out = *entry;
            return entry;
        }
    }
    if (D_8009AFC0 > 0) printf(D_80011EE8, component);
    return 0;
}

static int _cmp(char *left, char *right) {
    return strncmp(left, right, 12) == 0;
}

static int DS_newmedia(void) {
    u8 *cursor;
    u8 *end;
    int sector;
    int count;
    int read_status;
    IsoPathRecord *record;
    DslDirectoryCacheEntry *entry;

    read_status = ds_read(1, 16, (int)D_800A52B0);
    if (read_status != 1) {
        if (D_8009AFC0 > 0) puts(D_80011EF8);
        return 0;
    }
    if (strncmp(((IsoVolumePathTable *)D_800A52B0)->identifier,
                D_80011F24, 5) != 0) {
        if (D_8009AFC0 > 0) puts(D_80011F2C);
        return 0;
    }
    memcpy(&sector, ((IsoVolumePathTable *)D_800A52B0)->pathTableSectorLE, 4);
    if (ds_read(1, sector, (int)D_800A52B0) != read_status) {
        if (D_8009AFC0 > 0) printf(D_80011F5C, sector);
        return 0;
    }
    if (D_8009AFC0 > 1) puts(D_80011F80);
    cursor = D_800A52B0;
    end = cursor + 2048;
    for (count = 0; cursor < end; count++) {
        record = (IsoPathRecord *)cursor;
        if (record->nameLength == 0) break;
        entry = &g_DslDirectoryCache[count];
        memcpy(&entry->sector, record->sectorLE, 4);
        entry->directoryId = count + 1;
        entry->parentDirectoryId = record->parentDirectoryLE[0];
        memcpy(entry->name, record->name, record->nameLength);
        entry->name[record->nameLength] = 0;
        cursor += record->nameLength + 8 + (record->nameLength & 1);
        if (D_8009AFC0 > 1)
            printf(D_80011FA0, entry->sector, entry->directoryId,
                   entry->parentDirectoryId, entry->name);
        if (count + 1 == DSL_MAX_DIR) { count++; break; }
    }
    if (count < DSL_MAX_DIR) g_DslDirectoryCache[count].parentDirectoryId = 0;
    D_8009B6DC = 0;
    if (D_8009AFC0 > 1) printf(D_80011FB4, count);
    return 1;
}

static int DS_searchdir(int parent, char *name) {
    int i;
    for (i = 0; i < DSL_MAX_DIR; i++) {
        if (!g_DslDirectoryCache[i].parentDirectoryId) break;
        if (g_DslDirectoryCache[i].parentDirectoryId == parent &&
            strcmp(name, g_DslDirectoryCache[i].name) == 0)
            return i + 1;
    }
    return -1;
}

static int DS_cachefile(int directory) {
    u8 *cursor;
    int sector;
    int count;
    IsoDirectoryRecord *record;
    DslFILE *entry;

    if (D_8009B6DC == directory) return 1;
    if (ds_read(1, g_DslDirectoryCache[directory - 1].sector,
                (int)D_800A52B0) != 1) {
        if (D_8009AFC0 > 0) puts(D_80011FD8);
        return -1;
    }
    if (D_8009AFC0 > 1) puts(D_80011FF8);
    cursor = D_800A52B0;
    for (count = 0; cursor < D_800A52B0 + 2048; count++) {
        record = (IsoDirectoryRecord *)cursor;
        if (record->recordLength == 0) break;
        entry = &g_DslFileCache[count];
        memcpy(&sector, record->sectorLE, 4);
        CdIntToPos(sector, &entry->pos);
        memcpy(&entry->size, record->sizeLE, 4);
        if (count == 0) {
            memcpy(entry->name, D_80012014, 2);
        } else if (count == 1) {
            memcpy(entry->name, D_80012018, 3);
        } else {
            memcpy(entry->name, record->name, record->nameLength);
            entry->name[record->nameLength] = 0;
        }
        if (D_8009AFC0 > 1)
            printf(D_8001201C, entry->pos.minute, entry->pos.second,
                   entry->pos.sector, entry->size, entry->name);
        cursor += record->recordLength;
        if (count + 1 == DSL_MAX_FILE) { count++; break; }
    }
    D_8009B6DC = directory;
    if (count < DSL_MAX_FILE) g_DslFileCache[count].name[0] = 0;
    if (D_8009AFC0 > 1) printf(D_80012038, count);
    return 1;
}

static int ds_read(int count, int sector, int destination) {
    CdlLOC position;
    int status;

    CdIntToPos(sector, &position);
    CdRom_StartRead(&position, count, destination, 0x80);
    do {
        status = Sys_VSyncTimeout(0);
    } while (status > 0);

    return status == 0;
}
