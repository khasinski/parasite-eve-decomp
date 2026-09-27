#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
extern u8 D_800B0CE2[], D_8009D25C[];
extern u32 D_800B0DD8[];
extern u16 D_800930D8[], D_800930DA[];
/* Matching debt: four pins, six empty barriers, one retry jump, and a
 * 32-byte unused stack reserve retain retail allocation and scheduling.
 * The shifted state view preserves the bank-row address calculation. */
int Scene_LoadEntityTexture(void)
{
    int bank = *D_800B0CE2;
    u32 lba = *D_800B0DD8;
    Pe1GameState *state = &g_GameState;
    SceneAssetBlob *blob;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    register TimUploadRecord *firstTim asm("$4");
    register u32 timOffset asm("$2");
    SceneBankAssetRecord *record;
    register SceneBankAssetRecord *firstRecord asm("$3");
    register unsigned i asm("$16");
    u32 packed;
    u32 offsetMask;
    int index;
    int result;
    volatile u32 matchingStackReserve[8];
retry:
    switch (state->entity_texture_phase) {
    case 0:
        if (state->flags & 0x200000)
            state->entity_texture_phase = 1;
        else
            state->entity_texture_phase = 6;
        break;
    case 1:
        index = bank + 3;
        asm("" : "=r"(index) : "0"(index));
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[index],
                state->scene_load_scratch,
                D_800930DA[index] - D_800930D8[index]) != -1)
            state->entity_texture_phase = 2;
        return 1;
    case 2:
        result = CdRom_PollReady();
        if (result == -1) {
            state->entity_texture_phase = 1;
            return 1;
        }
        if (result != 0)
            return 1;
        if (state->flags & 0x20000) {
            if (!(state->flags & 0x80000) && *D_8009D25C < 2)
                return 1;
            state->flags |= 0x40000;
        }
        state->entity_texture_phase = 3;
        break;
    case 3:
        blob = state->scene_load_scratch;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        timOffset = directory->timEntries & 0x3fffff;
        asm("" : "=r"(timOffset) : "0"(timOffset));
        firstTim = SceneAsset_ResolveOffset(blob, timOffset);
        i = 0;
        if (directory->timEntries >> 22) {
            tim = firstTim;
            do {
                Gpu_LoadTimAsset(tim, blob);
                packed = directory->timEntries;
                asm volatile("" : "=r"(packed) : "0"(packed));
                ++i;
                ++tim;
            } while (i < (packed >> 22));
        }
        state->entity_texture_phase = 4;
        break;
    case 4:
        index = bank + 8;
        asm("" : "=r"(index) : "0"(index));
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[index],
                (void *)state->voice_bank_base_1400,
                D_800930DA[index] - D_800930D8[index]) != -1)
            state->entity_texture_phase = 5;
        return 1;
    case 5:
        result = CdRom_PollReady();
        if (result == -1) {
            state->entity_texture_phase = 4;
            return 1;
        }
        if (result != 0)
            return 1;
        state->entity_texture_phase = 6;
        break;
    case 6:
        blob = (SceneAssetBlob *)state->voice_bank_base_1400;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        {
            SceneBankAssetRecord *root = SceneAsset_ResolveOffset(blob, directory->bankRootEntries & 0x3fffff);
            state->bank_slots[0] = (u32)SceneAsset_ResolveOffset(blob, root->source.offsetAndId & 0xffffff);
        }
        firstRecord = SceneAsset_ResolveOffset(blob, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            offsetMask = 0xffffff;
            record = firstRecord;
            do {
                unsigned id;
                Pe1GameState *destination;
                asm("" : "=r"(record) : "0"(record), "r"(offsetMask));
                id = record->source.bytes.id;
                destination = (Pe1GameState *)(id * 4 + (u32)state);
                destination->bank_rows[0][0] = (u32)SceneAsset_ResolveOffset(
                    blob, record->source.offsetAndId & offsetMask);
                packed = directory->bankRowEntries;
                asm volatile("" : "=r"(packed) : "0"(packed));
                ++i;
                ++record;
            } while (i < (packed >> 22));
        }
        state->entity_texture_phase = 0;
        state->loaded_entity_bank = state->requested_entity_bank;
        state->flags &= ~0x200000;
        return 0;
    default:
        return 0;
    }
    goto retry;
}
