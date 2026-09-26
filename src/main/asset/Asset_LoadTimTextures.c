#include "pe1/scene_assets.h"
#include "pe1/psyq_tim.h"
#include "pe1/pm.h"
#include "pe1/cdrom.h"

static inline void InitHandler(int index)
{
    SceneAssetHandler *handler = g_PmCmdHandlerTable[index];
    if (handler && handler->init) handler->init();
}

int Asset_LoadTimTextures(int force)
{
    Pe1GameState *state = &g_GameState;
    int i;
    int status;
    RECT rect;

restart:
    switch (state->tim_load_state) {
    case 0:
        if (!(g_GameStateFlags & 0x80)) {
            /* Retail reloads the bank pointer on every iteration. Keep
             * those accesses explicit without changing its shared type. */
            (*(PmPrimarySlot *volatile *)&g_PmSlotTable) = state->scene_process_slots->primary;
            for (i = 0; i < 11; i++) {
                PmPrimarySlot *slot = &(*(PmPrimarySlot *volatile *)&g_PmSlotTable)[i];
                slot->header.state = 0;
                slot->header.owner = 0;
                slot->header.command = 255;
                slot->header.field02 = 255;
                slot->header.field03 = 255;
                slot->header.ticks = 0;
            }
            (*(PmSecondarySlot *volatile *)&g_PmSlotTable2) = state->scene_process_slots->secondary;
            for (i = 0; i < 11; i++) {
                PmSecondarySlot *slot = &(*(PmSecondarySlot *volatile *)&g_PmSlotTable2)[i];
                slot->header.state = 0;
                slot->header.owner = 0;
                slot->header.command = 255;
                slot->header.field02 = 255;
                slot->header.field03 = 255;
                slot->header.ticks = 0;
            }
            for (i = 0; i < 8; i++) InitHandler(i);
            InitHandler(85);
            {
                SceneAssetBlob *blob = state->loaded_scene_assets;
                SceneAssetDirectory *directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
                SceneAssetRecord *entry = SceneAsset_ResolveOffset(blob, directory->entries & 0x3FFFFF);
                for (i = 0; i < (int)(directory->entries >> 22); i++) {
                    unsigned int id = entry[i].handlerId;
                    if (id >= 8 && id < 85) InitHandler(id);
                }
            }
            state->flags = (state->flags & ~0x10000) | 8;
            g_GameStateFlags |= 0x80;
        }
        if ((force || (g_GameStateFlags & 2)) && (state->flags & 8)) {
            state->tim_load_state = 0x34;
            goto restart;
        }
        state->tim_load_state = 0;
        return 0;
    case 0x34:
        if (CdRom_ReadSectorsFromLba(state->pe_image_base_lba + D_800930E2,
                                   state->scene_load_scratch, D_800930E4 - D_800930E2) != -1)
            state->tim_load_state = 0x35;
        return 1;
    case 0x35:
        status = CdRom_PollReady();
        if (status == -1) {
            state->tim_load_state = 0x34;
            return 1;
        }
        if (status != 0) return 1;
        state->tim_load_state = 0x36;
        goto restart;
    case 0x36:
        {
            SceneAssetBlob *blob = state->scene_load_scratch;
            SceneAssetDirectory *directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
            TimUploadRecord *entry = SceneAsset_ResolveOffset(blob, directory->timEntries & 0x3FFFFF);
            u32 key;
            TimPackedImage *image;
            for (i = 0; i < (int)(directory->timEntries >> 22); i++)
                Gpu_LoadTimAsset(&entry[i], blob);
            blob = state->loaded_scene_assets;
            for (key = 0x73DECD80; ; key += 4) {
                image = (TimPackedImage *)Asset_FindTable08ByU32Key(blob, key);
                if (!image) break;
                rect.x = (image->geometry >> 10) & 0x7FF;
                rect.y = image->geometry >> 21;
                rect.w = image->geometry & 0x3FF;
                rect.h = image->source.bytes.height ? image->source.bytes.height : 256;
                LoadImage(&rect, SceneAsset_ResolveOffset(image, image->source.offsetAndHeight & 0xFFFFFF));
            }
            state->tim_load_state = 0;
            state->flags &= ~8;
            return 0;
        }
    default:
        return 0;
    }
}
