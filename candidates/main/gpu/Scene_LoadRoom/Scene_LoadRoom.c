/*
 * Scene_LoadRoom (0x8006B4F8, 2160 bytes): parked typed draft, lev 2.
 * Streams the three room ranges of PE.IMG, uploads the TIM lists of the
 * first two while the next read runs, then relocates the room directory
 * tables into g_GameState and queues the stream/sample bank reads.
 * See README.md for the remaining differences.
 */
/* CC1_FLAGS: -G1 */
/* MASPSX_FLAGS: -G1 */
#include "common.h"
#include "pe1/scene_room.h"
#include "pe1/cdrom.h"

int Scene_LoadRoom(unsigned int roomId)
{
    char name[8];
    unsigned int base;
    int map;
    int ready;
    int loaded;
    unsigned int i;
    unsigned int slot;
    int flags;
    SceneAssetView *room;
    SceneRoomDirectory *directory;
    TimUploadRecord *tim;
    SceneRoomRecord *record;
    SceneRoomShortRecord *shortRecord;
    Pe1GameState *state;

    name[0] = D_8009CDC8;
    memset(name + 1, 0, 6);
    flags = 0;
    state = &g_GameState;
    base = g_GameState.pe_image_base_lba;
    Str_EncodeBase32(name, roomId);

retry_scratch:
    {
        map = Str_ParseMapNumber(name) - 1;
        while ((ready = CdRom_ReadSectorsFromLba(base + D_80093378[map].start,
                                        state->scene_load_scratch,
                                        D_80093378[map].scratchSectors)) == -1) {
        }
        for (ready = 1; ready != 0; ready = CdRom_PollReady()) {
            if (ready == -1) {
                /* Recorded debt: CD retry restart (goto). */
                goto retry_scratch;
            }
        }
    }

    loaded = 0;
retry_texture:
    {
        while ((ready = CdRom_ReadSectorsFromLba(base + D_80093378[map].start +
                                            D_80093378[map].scratchSectors,
                                        state->texture_load_scratch,
                                        D_80093378[map].textureSectors)) == -1) {
        }
        for (ready = 1; ready != 0; ready = CdRom_PollReady()) {
            if (!loaded) {
                room = state->scene_load_scratch;
                directory = SceneAsset_ResolveOffset(room, room->header.directoryOffset);
                tim = SceneAsset_ResolveOffset(room, directory->tims & 0x3FFFFF);
                for (i = 0; i < directory->tims >> 22; i++) {
                    Gpu_LoadTimAsset(&tim[i], room);
                }
                loaded = 1;
            }
            if (ready == -1) {
                /* Recorded debt: CD retry restart (goto). */
                goto retry_texture;
            }
        }
    }

    loaded = 0;
retry_room:
    {
        while ((ready = CdRom_ReadSectorsFromLba(base + D_80093378[map].start +
                                            D_80093378[map].scratchSectors +
                                            D_80093378[map].textureSectors,
                                        state->loaded_scene_assets,
                                        D_80093378[map].roomSectors)) == -1) {
        }
        for (ready = 1; ready != 0; ready = CdRom_PollReady()) {
            if (!loaded) {
                room = state->texture_load_scratch;
                directory = SceneAsset_ResolveOffset(room, room->header.directoryOffset);
                tim = SceneAsset_ResolveOffset(room, directory->tims & 0x3FFFFF);
                for (i = 0; i < directory->tims >> 22; i++) {
                    Gpu_LoadTimAsset(&tim[i], room);
                }
                loaded = 1;
            }
            if (ready == -1) {
                /* Recorded debt: CD retry restart (goto). */
                goto retry_room;
            }
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

    room = state->loaded_scene_assets;
    directory = SceneAsset_ResolveOffset(room, room->header.directoryOffset);
    state->requested_entity_bank = directory->entityBank;
    state->room_type = directory->roomType;

    record = SceneAsset_ResolveOffset(room, directory->bankRoots & 0x3FFFFF);
    i = 0;
    if (directory->bankRoots >> 22) do {
        state->bank_slots[record[i].source.bytes.slot] =
            SCENE_ROOM_PAYLOAD(room, &record[i]);
    } while (++i < directory->bankRoots >> 22);

    record = SceneAsset_ResolveOffset(room, directory->bankRows & 0x3FFFFF);
    for (i = 0; i < directory->bankRows >> 22; i++) {
        state->bank_rows[record[i].u.row.group][record[i].source.bytes.slot] =
            SCENE_ROOM_PAYLOAD(room, &record[i]);
    }

    state->bank_reset_944[0] =
        Task_RelocBlock(SCENE_ROOM_PAYLOAD(room, SceneRoom_FirstRecord(room, directory->taskBlock)));
    state->bank_reset_948[0] = SCENE_ROOM_PAYLOAD(room, SceneRoom_FirstRecord(room, directory->taskData));
    state->bank_reset_94c[0] = SCENE_ROOM_PAYLOAD(room, SceneRoom_FirstRecord(room, directory->taskAux));

    shortRecord = SceneAsset_ResolveOffset(room, directory->taskSlots & 0x3FFFFF);
    for (i = 0; i < directory->taskSlots >> 22; i++) {
        state->bank_reset_950[shortRecord[i].source.bytes.slot] =
            SCENE_ROOM_PAYLOAD(room, &shortRecord[i]);
    }

    record = SceneAsset_ResolveOffset(room, directory->scripts & 0x3FFFFF);
    for (i = 0; i < directory->scripts >> 22; i++) {
        unsigned int index;

        slot = record[i].source.bytes.slot;
        index = slot;

        if (slot - 8 < 0x4D) {
            PmCommand **command = &g_PmCmdHandlerTable[slot];

            if (*command == 0) {
                *command = record[i].u.handler;
            }
        } else if (index >= 0x55) {
            void **handler;
            unsigned int entry = index - 0x55;

            handler = &D_800E1044[entry];
            if (*handler == 0) {
                *handler = record[i].u.handler;
            }
        }
    }

    if (directory->samples & 0xFFC00000) {
        SceneRoomRecord *sample = SceneAsset_ResolveOffset(room, directory->samples & 0x3FFFFF);
        for (i = 0; i < directory->samples >> 22; i++) {
            if (sample[i].flags & 0x20) {
                int key = sample[i].u.track.key;

                if (sample[i].u.track.key & 1) {
                    D_80094494[sample[i].source.bytes.slot] = key;
                    D_80094494[sample[i].source.bytes.slot + 8] = sample[i].u.track.key;
                } else {
                    D_80094494[sample[i].source.bytes.slot - 4] = key;
                    D_80094494[sample[i].source.bytes.slot + 4] = sample[i].u.track.key;
                }
            }
        }
    }

    if (directory->tracks & 0xFFC00000) {
        SceneRoomRecord *track = SceneAsset_ResolveOffset(room, directory->tracks & 0x3FFFFF);
        for (i = 0; i < directory->tracks >> 22; i++) {
            if (track[i].flags & 0x10) {
                state->scene_audio.tracks.pending_key = track[i].u.track.key;
                state->scene_audio.tracks.pending_bank = track[i].u.track.bank;
                state->bank_asset_table =
                    SceneAsset_ResolveOffset(state->loaded_scene_assets, track[i].source.offsetAndSlot & 0xFFFFFF);
            }
        }
    }

    if (directory->streams & 0xFFC00000) {
        SceneRoomStreamRecord *stream = SceneAsset_ResolveOffset(room, directory->streams & 0x3FFFFF);

        for (i = 0; i < directory->streams >> 22; i++) {
            if (stream[i].flags & 0x10) {
                continue;
            }
            if (stream[i].flags & 0x20) {
                if (stream[i].channel == 0) {
                    flags |= 0x10;
                    if (state->pending_stream_banks[0] != stream[i].bank.value) {
                        flags |= 1;
                        state->pending_stream_banks[0] = stream[i].bank.low;
                    }
                } else if (stream[i].channel == 1) {
                    flags |= 0x20;
                    if (state->pending_stream_banks[1] != stream[i].bank.value) {
                        flags |= 2;
                        state->pending_stream_banks[1] = stream[i].bank.low;
                    }
                }
            } else {
                u16 bank = stream[i].bank.value;

                flags |= 0x40;
                if (state->pending_sample_bank != bank) {
                    flags |= 4;
                    state->pending_sample_bank = bank;
                }
            }
        }
    }

    loaded = 1;
    if ((flags & 1) && state->pending_stream_banks[0] != 0) {
        CD_ReadSectors(2, state->pending_stream_banks[0], 0,
                       state->scene_load_scratch, 0x21, loaded);
    } else if (!(flags & 0x10)) {
        state->pending_stream_banks[0] = 0;
    }
    if ((flags & 2) && state->pending_stream_banks[1] != 0) {
        CD_ReadSectors(2, state->pending_stream_banks[1], 1,
                       state->scene_load_scratch, 0x21, 1);
    } else if (!(flags & 0x20)) {
        state->pending_stream_banks[1] = 0;
    }
    if ((flags & 4) && state->pending_sample_bank != -1) {
        CD_ReadSectors(1, state->pending_sample_bank, 0,
                       state->scene_load_scratch, 0x21, 1);
    } else if (!(flags & 0x40)) {
        state->pending_sample_bank = -1;
    }
    return 0;
}
