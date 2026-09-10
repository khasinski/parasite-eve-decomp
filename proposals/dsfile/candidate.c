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

static DslFILE file_cache[DSL_MAX_FILE] __asm__("D_800A36B0");
typedef struct DsStridedFileByte {
    u8 value;
    char remainder[sizeof(DslFILE) - sizeof(u8)];
} DsStridedFileByte;
extern DsStridedFileByte file_minutes[DSL_MAX_FILE] __asm__("D_800A36B0");
extern DsStridedFileByte file_seconds[DSL_MAX_FILE] __asm__("D_800A36B1");
extern DsStridedFileByte file_sectors[DSL_MAX_FILE] __asm__("D_800A36B2");
extern char file_cache_names[DSL_MAX_FILE][sizeof(DslFILE)]
    __asm__("D_800A36B8");
static DslDirectoryCacheEntry directory_cache[DSL_MAX_DIR] __asm__("D_800A3CB0");
typedef struct DsStridedParentId {
    int value;
    char remainder[sizeof(DslDirectoryCacheEntry) - sizeof(int)];
} DsStridedParentId;
typedef struct DsStridedDirectoryName {
    char value[32];
    char remainder[sizeof(DslDirectoryCacheEntry) - 32];
} DsStridedDirectoryName;
extern DsStridedParentId directory_parent_ids[DSL_MAX_DIR]
    __asm__("D_800A3CB4");
extern DsStridedDirectoryName directory_names[DSL_MAX_DIR]
    __asm__("D_800A3CBC");
static u8 sector_buffer[2048] __asm__("D_800A52B0");
static int cached_directory __asm__("D_8009B6DC") = 0;
extern int D_8009AFC0;

static const char path_level_error[] __asm__("D_80011E6C")
    __attribute__((aligned(4))) = "%s: path level (%d) error\n";
static const char directory_not_found[] __asm__("D_80011E88")
    __attribute__((aligned(4))) = "%s: dir was not found\n";
static const char search_disc_error[] __asm__("D_80011EA0")
    __attribute__((aligned(4))) = "DsSearchFile: disc error\n";
static const char search_progress[] __asm__("D_80011EBC")
    __attribute__((aligned(4))) = "DsSearchFile: searching %s...\n";
static const char search_found[] __asm__("D_80011EDC")
    __attribute__((aligned(4))) = "%s:  found\n";
static const char search_not_found[] __asm__("D_80011EE8")
    __attribute__((aligned(4))) = "%s: not found\n";
static const char volume_read_error[] __asm__("D_80011EF8")
    __attribute__((aligned(4))) = "DS_newmedia: Read error in ds_read(PVD)\n";
static const char volume_signature[] __asm__("D_80011F24")
    __attribute__((aligned(4))) = "CD001";
static const char volume_format_error[] __asm__("D_80011F2C")
    __attribute__((aligned(4))) = "DS_newmedia: Disc format error in ds_read(PVD)\n";
static const char path_table_read_error[] __asm__("D_80011F5C")
    __attribute__((aligned(4))) = "DS_newmedia: Read error (PT:%08x)\n";
static const char directory_progress[] __asm__("D_80011F80")
    __attribute__((aligned(4))) = "DS_newmedia: sarching dir..\n";
static const char directory_entry_format[] __asm__("D_80011FA0")
    __attribute__((aligned(4))) = "\t%08x,%04x,%04x,%s\n";
static const char directory_count_format[] __asm__("D_80011FB4")
    __attribute__((aligned(4))) = "DS_newmedia: %d dir entries found\n";
static const char directory_read_error[] __asm__("D_80011FD8")
    __attribute__((aligned(4))) = "DS_cachefile: dir not found\n";
static const char file_progress[] __asm__("D_80011FF8")
    __attribute__((aligned(4))) = "DS_cachefile: searching...\n";
static const char current_directory_name[] __asm__("D_80012014")
    __attribute__((aligned(4))) = ".";
static const char parent_directory_name[] __asm__("D_80012018")
    __attribute__((aligned(4))) = "..";
static const char file_entry_format[] __asm__("D_8001201C")
    __attribute__((aligned(4))) = "\t(%02x:%02x:%02x) %8d %s\n";
static const char file_count_format[] __asm__("D_80012038")
    __attribute__((aligned(4))) = "DS_cachefile: %d files found\n";

static int ds_read(int count, int sector, void *destination);
CdlLOC *DsIntToPos(int sector, CdlLOC *position) __asm__("CdIntToPos");
void *memcpy(void *destination, const void *source, unsigned int size);
int strncmp(const char *left, const char *right, unsigned int size);
int strcmp(const char *left, const char *right);
int printf(const char *format, ...);
int puts(const char *text);

static int cached_media_state __asm__("D_8009B6E0") = 0;
int DsShellOpen(void) __asm__("CdRom_GetDiskType");
int DsRead(CdlLOC *position, int count, u32 *destination, int mode)
    __asm__("CdRom_StartRead");
int DsReadSync(u_char *result) __asm__("Sys_VSyncTimeout");
static int DS_newmedia(void);
static int DS_searchdir(int parent, char *name);
static int DS_cachefile(int directory);
static int _cmp(char *left, char *right);

DslFILE *DsSearchFile(DslFILE *out, char *name) {
    char component[32];
    signed char *path;
    signed char *component_start;
    char *cursor;
    int depth, directory;
    int not_found;
    DslFILE *entry;
    char *entry_name;
    if (cached_media_state < DsShellOpen()) {
        if (!DS_newmedia()) return 0;
        cached_media_state = DsShellOpen();
    }
    if (*(signed char *)name != '\\') return 0;
    component[0] = 0;
    directory = 1;
    path = (signed char *)name;
    component_start = (signed char *)component;
    for (depth = 0; depth < 8; depth++) {
        cursor = component;
        not_found = -1;
        while (*path != '\\' && *path) *cursor++ = *path++;
        if (!*path) break;
        path++;
        *cursor = 0;
        directory = DS_searchdir(directory, component);
        if (directory == not_found) { component[0] = 0; break; }
    }
    if (depth >= 8) {
        if (D_8009AFC0 > 0) printf(path_level_error, name, depth);
        return 0;
    }
    if (!*component_start) {
        if (D_8009AFC0 > 0) printf(directory_not_found, name);
        return 0;
    }
    *cursor = 0;
    if (!DS_cachefile(directory)) {
        if (D_8009AFC0 > 0) puts(search_disc_error);
        return 0;
    }
    if (D_8009AFC0 > 1) printf(search_progress, component);
    for (depth = 0, entry = (DslFILE *)(file_cache_names[0] - 8),
         entry_name = file_cache_names[0];
         depth < 64;
         depth++, entry++, entry_name += sizeof(DslFILE)) {
        if (!*(signed char *)file_cache_names[depth]) break;
        if (_cmp(entry_name, component)) {
            if (D_8009AFC0 > 1) printf(search_found, component);
            *out = *entry;
            return entry;
        }
    }
    if (D_8009AFC0 > 0) printf(search_not_found, component);
    return 0;
}

static int _cmp(char *left, char *right) {
    return strncmp(left, right, 12) == 0;
}

static int DS_newmedia(void) {
    u8 *cursor;
    u8 *end;
    char *name;
    int sector;
    int count;
    int read_status;
    IsoPathRecord *record;

    read_status = ds_read(1, 16, sector_buffer);
    if (read_status != 1) {
        if (D_8009AFC0 > 0) puts(volume_read_error);
        return 0;
    }
    if (strncmp(((IsoVolumePathTable *)sector_buffer)->identifier,
                volume_signature, 5) != 0) {
        if (D_8009AFC0 > 0) puts(volume_format_error);
        return 0;
    }
    memcpy(&sector, ((IsoVolumePathTable *)sector_buffer)->pathTableSectorLE, 4);
    if (ds_read(1, sector, sector_buffer) != read_status) {
        if (D_8009AFC0 > 0) printf(path_table_read_error, sector);
        return 0;
    }
    if (D_8009AFC0 > 1) puts(directory_progress);
    cursor = sector_buffer;
    end = cursor + 2048;
    for (count = 0; cursor < end;) {
        record = (IsoPathRecord *)cursor;
        if (record->nameLength == 0) break;
        name = directory_cache[count].name;
        memcpy(&directory_cache[count].sector, record->sectorLE, 4);
        directory_parent_ids[count].value = record->parentDirectoryLE[0];
        directory_cache[count].directoryId = count + 1;
        memcpy(name, record->name, record->nameLength);
        name[record->nameLength] = 0;
        cursor += record->nameLength + 8 + (record->nameLength & 1);
        if (D_8009AFC0 > 1)
            printf(directory_entry_format, directory_cache[count].sector,
                   directory_cache[count].directoryId,
                   directory_parent_ids[count].value, name);
        if (++count >= DSL_MAX_DIR) break;
    }
    if (count < DSL_MAX_DIR) directory_parent_ids[count].value = 0;
    cached_directory = 0;
    if (D_8009AFC0 > 1) printf(directory_count_format, count);
    return 1;
}

static int DS_searchdir(int parent, char *name) {
    int i;
    for (i = 0; i < DSL_MAX_DIR; i++) {
        if (!directory_parent_ids[i].value) break;
        if (directory_parent_ids[i].value == parent &&
            strcmp(name, directory_names[i].value) == 0)
            return i + 1;
    }
    return -1;
}

static int DS_cachefile(int directory) {
    IsoDirectoryRecord *cursor;
    int sector;
    int count;

    if (directory == cached_directory) return 1;
    if (ds_read(1, (directory_cache + directory)[-1].sector,
                sector_buffer) != 1) {
        if (D_8009AFC0 > 0) puts(directory_read_error);
        return -1;
    }
    if (D_8009AFC0 > 1) puts(file_progress);
    cursor = (IsoDirectoryRecord *)sector_buffer;
    for (count = 0; (u8 *)cursor < sector_buffer + 2048;) {
        if (cursor->recordLength == 0) break;
        memcpy(&sector, cursor->sectorLE, 4);
        DsIntToPos(sector, &file_cache[count].pos);
        memcpy(&file_cache[count].size, cursor->sizeLE, 4);
        switch (count) {
        case 0:
            memcpy(file_cache[0].name, current_directory_name, 2);
            break;
        case 1:
            memcpy(file_cache[1].name, parent_directory_name, 3);
            break;
        default:
            memcpy(file_cache[count].name, cursor->name, cursor->nameLength);
            file_cache[count].name[cursor->nameLength] = 0;
        }
        if (D_8009AFC0 > 1)
            printf(file_entry_format, file_minutes[count].value,
                   file_seconds[count].value, file_sectors[count].value,
                   file_cache[count].size, file_cache[count].name);
        cursor = (IsoDirectoryRecord *)((u8 *)cursor + cursor->recordLength);
        if (++count >= DSL_MAX_FILE) break;
    }
    cached_directory = directory;
    if (count < DSL_MAX_FILE) file_cache[count].name[0] = 0;
    if (D_8009AFC0 > 1) printf(file_count_format, count);
    return 1;
}

static int ds_read(int count, int sector, void *destination) {
    CdlLOC position;
    int status;

    DsIntToPos(sector, &position);
    DsRead(&position, count, destination, 0x80);
    do {
        status = DsReadSync(0);
    } while (status > 0);

    return status == 0;
}
