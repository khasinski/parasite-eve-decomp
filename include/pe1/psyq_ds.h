#ifndef PE1_PSYQ_DS_H
#define PE1_PSYQ_DS_H

#include "pe1/psyq_cd.h"

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
int _cmp(char *left, char *right);
int strncmp(const char *left, const char *right, unsigned int count);
int strcmp(const char *left, const char *right);

#endif /* PE1_PSYQ_DS_H */
