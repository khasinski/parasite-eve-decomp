#include "common.h"
#include "pe1/boot_disc_check.h"
#include "pe1/scene_assets.h"
#define NULL ((void *)0)

#include "pe1/psyq_gpu.h"
#include "pe1/cdrom.h"

typedef struct { char b[16]; } __attribute__((aligned(1), packed)) Copy16u;
typedef struct { u32 w[4]; } Copy16a;

void Menu_FullInit(void);
void Spu_UploadSampleBlockBlocking(void *arg0, int arg1);

extern struct { char _[16]; } g_PeImageBaseLba_o __asm__("g_PeImageBaseLba");
extern struct { char _[16]; } D_800930DC_o __asm__("D_800930DC");
extern struct { char _[16]; } D_800930DE_o __asm__("D_800930DE");
extern struct { char _[16]; } D_800930E4_o __asm__("D_800930E4");
extern struct { char _[16]; } D_800930E6_o __asm__("D_800930E6");
extern struct { char _[16]; } D_800A8028_o __asm__("g_GlyphMetricsTable");
extern struct { char _[16]; } D_800E2858_o __asm__("D_800E2858");
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
extern struct { char _[16]; } D_800B0E6C_o __asm__("g_SceneLoadScratchBuffer");

void Scene_LoadSceneData(void) {
    RECT rect;
    Pe1GameState *state;
    s32 lba;
    u8 *p0;
    s32 t;
    s32 flag;
    u8 *src;
    u8 *dst;
    u8 *end;
    register u8 *copyEnd asm("$8");

    rect.w = 0x3FF;
    rect.x = 0;
    rect.y = 0;
    rect.h = 0x1FF;
    state = (Pe1GameState *)&D_800B0CD8_o;
    lba = *(s32 *)&g_PeImageBaseLba_o;
    ClearImage(&rect, 0, 0, 1);

restart1:
    p0 = (u8 *)&D_800930DC_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, (void *)(s32)&D_800A8028_o, *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (t == -1) {
            goto restart1;
        }
        t = CdRom_PollReady();
    } while (t != 0);

    flag = 0;
restart2:
    p0 = (u8 *)&D_800930DE_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, state->scene_load_scratch, *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (flag == 0) {
            Menu_FullInit();
            flag = 1;
        }
        if (t == -1) {
            goto restart2;
        }
        t = CdRom_PollReady();
    } while (t != 0);

    src = state->scene_load_scratch;
    dst = (u8 *)&D_800E2858_o;
    if (((u32)src | (u32)dst) & 3) {
        copyEnd = src + 0x10A50;
        do {
            *(Copy16u *)dst = *(Copy16u *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != copyEnd);
    } else {
        copyEnd = src + 0x10A50;
        do {
            *(Copy16a *)dst = *(Copy16a *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != copyEnd);
    }

    state->startup_resource_blob = (u8 *)&D_800E2858_o;
    state->startup_resource_tables[0] = Asset_FindTable08ByU32Key(&D_800E2858_o, 0x57D40D84);
    state->startup_resource_tables[1] = Asset_FindTable08ByU32Key(state->startup_resource_blob, 0x57D41D84);

restart3:
    p0 = (u8 *)&D_800930E4_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, state->scene_load_scratch, *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (t == -1) {
            goto restart3;
        }
        t = CdRom_PollReady() != 0;
    } while (t != 0);

    Spu_UploadSampleBlockBlocking(*(s32 *)&D_800B0E6C_o, 1);

restart4:
    p0 = (u8 *)&D_800930E6_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, state->scene_load_scratch, *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (t == -1) {
            goto restart4;
        }
    } while ((t = (CdRom_PollReady() != 0)) != 0);

    dst = (u8 *)state->bank_work_far_end;
    __asm__ __volatile__("" : "=r"(dst) : "0"(dst));
    src = state->scene_load_scratch;
    end = src + 0x1400;
    if (((u32)src | (u32)dst) & 3) {
        do {
            *(Copy16u *)dst = *(Copy16u *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != end);
    } else {
        do {
            *(Copy16a *)dst = *(Copy16a *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != end);
    }
}

/* The adjacent field background loader shares the scene resource state. */
#include "pe1/game_state.h"
#include "pe1/pe_image.h"
#include "pe1/asset_tim.h"
#include "pe1/menu_memcard_display.h"
#include "pe1/render_prim.h"
#include "pe1/field_bg_load.h"

/* Uploads every TIM listed in a scene blob's directory. */
#define SCENE_BLOB_LOAD_TIMS(source)                                         \
    {                                                                        \
        unsigned int i;  \
                                                                             \
        blob = (source);                                                     \
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);   \
        tim = SceneAsset_ResolveOffset(blob, directory->timEntries & 0x3fffff); \
        for (i = 0; i < directory->timEntries >> 22; i++)                    \
            Gpu_LoadTimAsset(&tim[i], blob);                                 \
    }

int Scene_LoadFieldBg(void)
{
    Pe1GameState *state = &g_GameState;
    u32 lba;
    unsigned int slot;
    FieldBgImageRecord *image;
    SceneAssetBlob *blob;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    int status;
    int done;

    if (!(state->flags & 1))
        return 0;

    lba = g_PeImageBaseLba;

retry1:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[9], state->bg_texture_blob,
               D_800930D8[10] - D_800930D8[9]) == -1)
        ;
    status = 1;
    do {
        if (status == -1)
            goto retry1; /* recorded debt: CD retry restart */
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry2:
    while ((status = CdRom_ReadSectorsFromLba(lba + D_800930D8[10], state->bg_tim,
               D_800930D8[11] - D_800930D8[10])) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            SCENE_BLOB_LOAD_TIMS(state->bg_texture_blob);
            for (slot = 2; slot < 4; slot++) {
                D_80091648[slot].tpage = BG_SLOT_TPAGE(D_80091648[slot].x, D_80091648[slot].y);
                D_80091648[slot].clut = BG_SLOT_CLUT(D_80091648[slot].clut_x, D_80091648[slot].clut_y);
            }
            image = Asset_FindTable08ByU32Key(blob, 0xABADC06C);
            while (image->length != 0) {
                LoadImage(&image->rect, image->pixels);
                image = (FieldBgImageRecord *)(&image->length + (image->length >> 2));
            }
            done = 1;
        }
        if (status == -1)
            goto retry2; /* recorded debt: CD retry restart */
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry3:
    while ((status = CdRom_ReadSectorsFromLba(lba + D_800930D8[11], state->hud_tim,
               D_800930D8[12] - D_800930D8[11])) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            Asset_LoadTimImage(state->bg_tim);
            for (slot = 0; slot < 2; slot++) {
                D_80091648[slot].tpage = BG_SLOT_TPAGE(D_80091648[slot].x, D_80091648[slot].y);
                D_80091648[slot].clut = BG_SLOT_CLUT(D_80091648[slot].clut_x, D_80091648[slot].clut_y);
            }
            done = 1;
        }
        if (status == -1)
            goto retry3; /* recorded debt: CD retry restart */
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry4:
    while ((status = CdRom_ReadSectorsFromLba(lba + D_800930D8[12], state->bank_asset_source,
               D_800930D8[13] - D_800930D8[12])) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            done = 1;
            Asset_LoadTimImage(state->hud_tim);
            Battle_DrawHPBar();
        }
        if (status == -1)
            goto retry4; /* recorded debt: CD retry restart */
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry5:
    while ((status = CdRom_ReadSectorsFromLba(lba + D_800930D8[4], state->object_texture_blob,
               D_800930D8[5] - D_800930D8[4])) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            state->scene_object_model =
                Asset_FindTable08ByU32Key(state->bank_asset_source, 0xC4B5BA04);
            done = 1;
            state->room_geometry_table =
                Asset_FindTable08ByU32Key(state->bank_asset_source, 0xCAAD0704);
            state->bank_asset_table =
                Asset_FindTable08ByU32Key(state->bank_asset_source, 0x5EAF6804);
        }
        if (status == -1)
            goto retry5; /* recorded debt: CD retry restart */
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry6:
    while ((status = CdRom_ReadSectorsFromLba(lba + D_800930D8[39], state->scene_process_slots,
               D_800930D8[40] - D_800930D8[39])) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            SCENE_BLOB_LOAD_TIMS(state->object_texture_blob);
            done = 1;
        }
        if (status == -1)
            goto retry6; /* recorded debt: CD retry restart */
        status = CdRom_PollReady();
    } while (status != 0);

    {
        unsigned int i;

        blob = state->scene_process_slots;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        tim = SceneAsset_ResolveOffset(blob, directory->timEntries & 0x3fffff);
        i = 0;
        if (directory->timEntries >> 22) {
            do {
                Gpu_LoadTimAsset(&tim[i], blob);
                i++;
            } while (i < directory->timEntries >> 22);
        }
    }

    Akao_Cmd_F1();
    DrawSync(0);
    ResetGraph(1);
    VSync(0);
    PutDispEnv(&D_800BCE80[D_8009CDDC]);
    SetDispMask(1);
    state->field_bg_cache_id = -1;
    state->loaded_entity_bank = 0;
    state->current_story_day = -1;
    state->field_bg_cache_bank = -1;
    state->pending_sample_bank = -1;
    state->pending_stream_banks[1] = 0;
    state->pending_stream_banks[0] = 0;
    if (!(state->flags & 0x40)) {
        state->scene_audio.tracks.banks[0] = -1;
        state->scene_audio.tracks.keys[0][1] = -1;
        state->scene_audio.tracks.keys[0][0] = -1;
    }
    if (!(state->flags & 0x80)) {
        state->scene_audio.tracks.banks[1] = -1;
        state->scene_audio.tracks.keys[1][1] = -1;
        state->scene_audio.tracks.keys[1][0] = -1;
    }
    state->flags &= ~1;
    return 0;
}
