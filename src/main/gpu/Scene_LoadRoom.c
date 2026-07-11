typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009CDC8_o __asm__("D_8009CDC8");
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
extern struct { char _[16]; } D_800B0DD8_o __asm__("D_800B0DD8");
extern u32 D_80093378[];
extern u32 D_8009337C[];
extern u32 D_800942E0[];
extern u32 D_800E1044[];
extern u16 D_80094494[];

#define D_8009CDC8 (*(u8 *)&D_8009CDC8_o)
#define GAME ((u8 *)&D_800B0CD8_o)
#define D_800B0DD8 (*(s32 *)&D_800B0DD8_o)

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

void memset(void *dst, int value, unsigned int size);
int Str_EncodeBase32(char *out, unsigned int value);
int Str_ParseMapNumber(char *name);
int CdRom_ReadSectorsFromLba(int lba, void *dst, int sectors);
int CdRom_PollReady(void);
int Gpu_LoadTimAsset(void *tim, void *base);
void EnterCriticalSection(void);
void FlushCache(void);
void ExitCriticalSection(void);
void *Task_RelocBlock(void *block);
int CD_ReadSectors(int mode, int a1, int a2, void *dst, int sectors, int flag);

static int Scene_RoomRangeStart(int map_index) {
    return D_80093378[map_index];
}

static int Scene_RoomRangePacked(int map_index) {
    return D_8009337C[map_index];
}

static int Scene_RoomRangePrefixSectors(int map_index) {
    return (u8)D_8009337C[map_index];
}

static int Scene_RoomRangeMiddleSectors(int map_index) {
    return (D_8009337C[map_index] >> 8) & 0xFFF;
}

static int Scene_RoomRangeTailSectors(int map_index) {
    return D_8009337C[map_index] >> 20;
}

static int Scene_EntryCount(u32 packed) {
    return packed >> 22;
}

static u8 *Scene_EntryBase(u8 *room, u32 packed) {
    return room + (packed & 0x3FFFFF);
}

static u8 *Scene_EntryPointer24(u8 *room, u8 *entry) {
    return room + (U32_AT(entry, 4) & 0xFFFFFF);
}

static void Scene_LoadTimList(u8 *room, u32 packed) {
    u8 *entry;
    int i;
    int count;

    entry = Scene_EntryBase(room, packed);
    count = Scene_EntryCount(packed);
    for (i = 0; i < count; i++) {
        Gpu_LoadTimAsset(entry, room);
        entry += 0x14;
    }
}

static void Scene_RelocPointerTable(u8 *room, u32 packed, int game_offset) {
    u8 *entry;
    int i;
    int count;

    entry = Scene_EntryBase(room, packed);
    count = Scene_EntryCount(packed);
    for (i = 0; i < count; i++) {
        PTR_AT(GAME, game_offset + U8_AT(entry, 7) * 4) = Scene_EntryPointer24(room, entry);
        entry += 0x0C;
    }
}

static void Scene_RelocGroupedPointerTable(u8 *room, u32 packed) {
    u8 *entry;
    int i;
    int count;

    entry = Scene_EntryBase(room, packed);
    count = Scene_EntryCount(packed);
    for (i = 0; i < count; i++) {
        u8 group;
        u8 slot;

        slot = U8_AT(entry, 7);
        group = U8_AT(entry, 0x0B);
        PTR_AT(GAME, 0x1C0 + group * 0xC0 + slot * 4) = Scene_EntryPointer24(room, entry);
        entry += 0x0C;
    }
}

static void Scene_RelocPointerTable8(u8 *room, u32 packed, int game_offset) {
    u8 *entry;
    int i;
    int count;

    entry = Scene_EntryBase(room, packed);
    count = Scene_EntryCount(packed);
    for (i = 0; i < count; i++) {
        PTR_AT(GAME, game_offset + U8_AT(entry, 7) * 4) = Scene_EntryPointer24(room, entry);
        entry += 8;
    }
}

static void Scene_RelocTaskBlock(u8 *room, u8 *header) {
    u8 *entry;

    entry = Scene_EntryBase(room, U32_AT(header, 0x14));
    PTR_AT(GAME, 0x944) = Task_RelocBlock(Scene_EntryPointer24(room, entry));

    entry = Scene_EntryBase(room, U32_AT(header, 0x18));
    PTR_AT(GAME, 0x948) = Scene_EntryPointer24(room, entry);

    entry = Scene_EntryBase(room, U32_AT(header, 0x1C));
    PTR_AT(GAME, 0x94C) = Scene_EntryPointer24(room, entry);
}

static void Scene_RegisterScriptPointers(u8 *room, u32 packed) {
    u8 *entry;
    int i;
    int count;

    entry = Scene_EntryBase(room, packed);
    count = Scene_EntryCount(packed);
    for (i = 0; i < count; i++) {
        u8 slot;
        u32 *table;

        slot = U8_AT(entry, 7);
        if ((unsigned int)(slot - 8) < 0x4D) {
            table = D_800942E0 + slot;
        } else if (slot >= 0x55) {
            table = D_800E1044 + (slot - 0x55);
        } else {
            table = 0;
        }

        if (table != 0 && *table == 0) {
            *table = U32_AT(entry, 8);
        }
        entry += 0x0C;
    }
}

static int Scene_UpdateDynamicResourceState(u8 *room, u8 *header) {
    u8 *entry;
    int i;
    int count;
    int flags;

    flags = 0;

    if (U32_AT(header, 0x2C) & 0xFFC00000) {
        entry = Scene_EntryBase(room, U32_AT(header, 0x2C));
        count = Scene_EntryCount(U32_AT(header, 0x2C));
        for (i = 0; i < count; i++) {
            if (U8_AT(entry, 3) & 0x20) {
                u16 value = U16_AT(entry, 0x0A);
                u8 slot = U8_AT(entry, 7);

                if (value & 1) {
                    D_80094494[slot] = value;
                    D_80094494[slot + 8] = value;
                } else {
                    D_80094494[slot - 4] = value;
                    D_80094494[slot + 4] = value;
                }
            }
            entry += 0x0C;
        }
    }

    if (U32_AT(header, 0x30) & 0xFFC00000) {
        entry = Scene_EntryBase(room, U32_AT(header, 0x30));
        count = Scene_EntryCount(U32_AT(header, 0x30));
        for (i = 0; i < count; i++) {
            if (U8_AT(entry, 3) & 0x10) {
                U8_AT(GAME, 0xE0) = U16_AT(entry, 0x0A);
                U8_AT(GAME, 0xE1) = U8_AT(entry, 8);
                PTR_AT(GAME, 0x124) = PTR_AT(GAME, 0x18C) + (U32_AT(entry, 4) & 0xFFFFFF);
            }
            entry += 0x0C;
        }
    }

    if (U32_AT(header, 0x24) & 0xFFC00000) {
        entry = Scene_EntryBase(room, U32_AT(header, 0x24));
        count = Scene_EntryCount(U32_AT(header, 0x24));
        for (i = 0; i < count; i++) {
            u8 attr;
            int value;

            attr = U8_AT(entry, 3);
            if (attr & 0x10) {
                continue;
            }

            if (attr & 0x20) {
                value = U16_AT(entry, 4);
                if (U16_AT(entry, 6) == 0) {
                    flags |= 0x10;
                    if (U8_AT(GAME, 0xEA) != value) {
                        flags |= 1;
                        U8_AT(GAME, 0xEA) = value;
                    }
                } else if (U16_AT(entry, 6) == 1) {
                    flags |= 0x20;
                    if (U8_AT(GAME, 0xEB) != value) {
                        flags |= 2;
                        U8_AT(GAME, 0xEB) = value;
                    }
                }
            } else {
                value = U16_AT(entry, 4);
                flags |= 0x40;
                if (S16_AT(GAME, 0xE8) != value) {
                    flags |= 4;
                    S16_AT(GAME, 0xE8) = value;
                }
            }
            entry += 8;
        }
    }

    return flags;
}

static void Scene_IssueDynamicReads(int flags) {
    if (flags & 1) {
        if (U8_AT(GAME, 0xEA) != 0) {
            CD_ReadSectors(2, U8_AT(GAME, 0xEA), 0, PTR_AT(GAME, 0x194), 0x21, 1);
        }
    } else if (!(flags & 0x10)) {
        U8_AT(GAME, 0xEA) = 0;
    }

    if (flags & 2) {
        if (U8_AT(GAME, 0xEB) != 0) {
            CD_ReadSectors(2, U8_AT(GAME, 0xEB), 1, PTR_AT(GAME, 0x194), 0x21, 1);
        }
    } else if (!(flags & 0x20)) {
        U8_AT(GAME, 0xEB) = 0;
    }

    if (flags & 4) {
        if (S16_AT(GAME, 0xE8) != -1) {
            CD_ReadSectors(1, S16_AT(GAME, 0xE8), 0, PTR_AT(GAME, 0x194), 0x21, 1);
        }
    } else if (!(flags & 0x40)) {
        S16_AT(GAME, 0xE8) = -1;
    }
}

static void Scene_WaitForCdReady(int retry_code) {
    int ready;

    do {
        ready = CdRom_PollReady();
    } while (ready != 0 && ready != retry_code);
}

int Scene_LoadRoom(unsigned int room_id) {
    char room_name[8];
    int map_index;
    int lba;
    int retry_code;
    u8 *room;
    u8 *header;
    int loaded_tim;
    int flags;

    room_name[0] = D_8009CDC8;
    memset(room_name + 1, 0, 6);
    Str_EncodeBase32(room_name, room_id);

    map_index = Str_ParseMapNumber(room_name) - 1;
    lba = D_800B0DD8 + Scene_RoomRangeStart(map_index);
    retry_code = -1;

    do {
        while (CdRom_ReadSectorsFromLba(lba, PTR_AT(GAME, 0x194),
                                        Scene_RoomRangePrefixSectors(map_index)) == retry_code) {}
        Scene_WaitForCdReady(retry_code);

        loaded_tim = 0;
        while (CdRom_ReadSectorsFromLba(lba + Scene_RoomRangePrefixSectors(map_index),
                                        PTR_AT(GAME, 0x168),
                                        Scene_RoomRangeMiddleSectors(map_index)) == retry_code) {}
        do {
            if (!loaded_tim) {
                room = PTR_AT(GAME, 0x194);
                header = room + U32_AT(room, 4);
                Scene_LoadTimList(room, U32_AT(header, 0x28));
                loaded_tim = 1;
            }
        } while (CdRom_PollReady() != 0);

        loaded_tim = 0;
        while (CdRom_ReadSectorsFromLba(lba + Scene_RoomRangePrefixSectors(map_index) +
                                            Scene_RoomRangeMiddleSectors(map_index),
                                        PTR_AT(GAME, 0x18C),
                                        Scene_RoomRangeTailSectors(map_index)) == retry_code) {}
        do {
            if (!loaded_tim) {
                room = PTR_AT(GAME, 0x168);
                header = room + U32_AT(room, 4);
                Scene_LoadTimList(room, U32_AT(header, 0x28));
                loaded_tim = 1;
            }
        } while (CdRom_PollReady() != 0);
    } while (0);

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

    room = PTR_AT(GAME, 0x18C);
    header = room + U32_AT(room, 4);
    U8_AT(GAME, 0x0A) = U8_AT(header, 1);
    U8_AT(GAME, 0x08) = U8_AT(header, 3);

    Scene_RelocPointerTable(room, U32_AT(header, 0x0C), 0x198);
    Scene_RelocGroupedPointerTable(room, U32_AT(header, 0x10));
    Scene_RelocTaskBlock(room, header);
    Scene_RelocPointerTable8(room, U32_AT(header, 0x20), 0x950);
    Scene_RegisterScriptPointers(room, U32_AT(header, 4));
    flags = Scene_UpdateDynamicResourceState(room, header);
    Scene_IssueDynamicReads(flags);

    return 0;
}
