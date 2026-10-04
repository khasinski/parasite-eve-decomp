#ifndef PE1_SCENE_ROOM_H
#define PE1_SCENE_ROOM_H

#include "common.h"
#include "pe1/scene_assets.h"

/*
 * Room archive loaded by Scene_LoadRoom. The archive starts with a
 * SceneAssetBlob whose directory words pack a 22-bit record table offset
 * with a 10-bit record count.
 */
typedef struct SceneRoomDirectory {
    u8 reserved00;
    u8 entityBank;          /* 0x01: copied to requested_entity_bank */
    u8 reserved02;
    u8 roomType;            /* 0x03 */
    unsigned int scripts;   /* 0x04: handler records */
    unsigned int reserved08;
    unsigned int bankRoots; /* 0x0C */
    unsigned int bankRows;  /* 0x10 */
    unsigned int taskBlock; /* 0x14 */
    unsigned int taskData;  /* 0x18 */
    unsigned int taskAux;   /* 0x1C */
    unsigned int taskSlots; /* 0x20: short records */
    unsigned int streams;   /* 0x24: stream records */
    unsigned int tims;      /* 0x28 */
    unsigned int samples;   /* 0x2C */
    unsigned int tracks;    /* 0x30 */
} SceneRoomDirectory;

/* Low 24 bits: payload offset from the archive base; top byte: slot. */
typedef union SceneRoomPayload {
    u32 offsetAndSlot;
    struct {
        u8 offset[3];
        u8 slot;
    } bytes;
} SceneRoomPayload;

/* Twelve-byte record: the payload word keeps a 24-bit offset and a slot byte. */
typedef struct SceneRoomRecord {
    u8 reserved[3];
    u8 flags;             /* 0x03 */
    SceneRoomPayload source; /* 0x04 */
    union {
        void *handler;    /* 0x08: script handler */
        struct {
            u8 bank;      /* 0x08 */
            u8 reserved;
            u16 key;      /* 0x0A */
        } track;
        struct {
            u8 reserved[3];
            u8 group;     /* 0x0B */
        } row;
    } u;
} SceneRoomRecord;

/* Eight-byte variant without the trailing word. */
typedef struct SceneRoomShortRecord {
    u8 reserved[3];
    u8 flags;
    SceneRoomPayload source;
} SceneRoomShortRecord;

/* Stream selection record: bank number and target channel. */
typedef struct SceneRoomStreamRecord {
    u8 reserved[3];
    u8 flags;     /* 0x03: 0x10 skip, 0x20 stream (else sample bank) */
    union {
        u16 value; /* 0x04: bank number */
        u8 low;    /* stream banks are stored back as bytes */
    } bank;
    u16 channel;  /* 0x06 */
} SceneRoomStreamRecord;

/* Sector range of one map inside PE.IMG, split in three consecutive reads. */
typedef struct SceneRoomSectorRange {
    u32 start;
    unsigned int scratchSectors : 8;
    unsigned int textureSectors : 12;
    unsigned int roomSectors : 12;
} SceneRoomSectorRange;

/*
 * Payload pointer of a room record: the low 24 bits of its payload word are a
 * byte offset into the room archive. A macro rather than
 * SceneAsset_ResolveOffset: retail computes the destination slot before the
 * payload, which an inline call on the right-hand side would reorder.
 */
#define SCENE_ROOM_PAYLOAD(view, record) \
    SCENE_ASSET_AT(view, (record)->source.offsetAndSlot & 0xFFFFFF)

/* First record of a directory table (low 22 bits of the packed word). */
static inline SceneRoomRecord *SceneRoom_FirstRecord(SceneAssetView *room, unsigned int packed)
{
    return SceneAsset_ResolveOffset(room, packed & 0x3FFFFF);
}

PE1_STATIC_ASSERT(sizeof(SceneRoomDirectory) == 0x34, scene_room_directory_size);
PE1_STATIC_ASSERT(sizeof(SceneRoomRecord) == 12, scene_room_record_size);
PE1_STATIC_ASSERT(sizeof(SceneRoomShortRecord) == 8, scene_room_short_record_size);
PE1_STATIC_ASSERT(sizeof(SceneRoomStreamRecord) == 8, scene_room_stream_record_size);
PE1_STATIC_ASSERT(sizeof(SceneRoomSectorRange) == 8, scene_room_sector_range_size);

extern u8 D_8009CDC8; /* room name prefix letter */
extern SceneRoomSectorRange D_80093378[];
extern u16 D_80094494[];
extern unsigned int g_PeImageBaseLba;

void *memset(void *dst, int value, unsigned int size);
int Str_EncodeBase32(char *out, unsigned int value);
int Str_ParseMapNumber(char *name);
void *Task_RelocBlock(void *block);
void EnterCriticalSection(void);
void ExitCriticalSection(void);
void FlushCache(void);

#endif
