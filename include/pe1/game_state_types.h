#ifndef PE1_GAME_STATE_TYPES_H
#define PE1_GAME_STATE_TYPES_H

typedef signed char Pe1S8;
typedef unsigned char Pe1U8;
typedef unsigned int Pe1U32;

/*
 * Shared scene, field, and AKAO state at 0x800B0CD8 (g_GameState).
 * Only fields corroborated by multiple main-executable users are named;
 * padding keeps the observed retail offsets stable while the remaining
 * substructures are reconstructed.
 */
struct SceneAssetBlob;
struct PmSlotBanks;
struct RenderObjectEntity;
union SceneAssetView;

/* Embedded render object at 0x014; the full layout is RenderObjectEntity. */
typedef struct Pe1SceneRenderObject {
    void *sections[9];                             /* 0x00 */
    struct RenderObjectEntity *animation_source;   /* 0x24 */
    short animation_state, animation_id;           /* 0x28 */
    Pe1U8 reserved_2c[0x5C];
    Pe1U8 shade, light_negative_y, light_positive_y; /* 0x88 */
    Pe1U8 reserved_8b[0x39];
} Pe1SceneRenderObject;

typedef union Pe1SceneAudioState {
    Pe1U8 bytes[0x10];
    struct {
        Pe1U8 reserved[2];
        Pe1U8 banks[2];
        Pe1S8 keys[2][2];
        Pe1S8 pending_key, pending_bank; /* 0xE0, 0xE1 in game state */
        Pe1U8 trailing[6];
    } tracks;
} Pe1SceneAudioState;

typedef struct Pe1GameState {
    Pe1U32 flags;                    /* 0x000 */
    Pe1U8 unk_004[4];
    Pe1U8 room_type;                /* 0x008: room directory byte 3 */
    Pe1U8 unk_009;
    Pe1U8 requested_entity_bank;    /* 0x00A */
    Pe1U8 loaded_entity_bank;       /* 0x00B */
    Pe1S8 current_story_day;         /* 0x00C */
    Pe1S8 pending_story_day;         /* 0x00D */
    Pe1U8 story_day_flags;           /* 0x00E */
    Pe1U8 unk_00f;
    Pe1U8 cd_range_read_mode;      /* 0x010: CD_FindNextDataSector */
    Pe1U8 bank_state_11, bank_state_12;
    Pe1U8 unk_013;
    Pe1SceneRenderObject scene_object; /* 0x014 */
    Pe1SceneAudioState scene_audio; /* 0x0D8: cached track banks and keys */
    short pending_sample_bank;      /* 0x0E8: -1 means no sample upload */
    Pe1U8 pending_stream_banks[2];  /* 0x0EA: zero means no stream upload */
    Pe1U8 entity_texture_phase;     /* 0x0EC */
    Pe1U8 scene_init_phase;      /* 0x0ED */
    Pe1U8 scene_init_subphase;   /* 0x0EE */
    Pe1U8 tim_load_state;            /* 0x0EF: 0, 0x34, 0x35, 0x36 */
    Pe1U8 cd_read_phase;            /* 0x0F0 */
    Pe1U8 cd_track_phase;           /* 0x0F1 */
    Pe1U8 cd_transition_phase;      /* 0x0F2 */
    Pe1U8 cd_range_state;          /* 0x0F3: CD_FindNextDataSector */
    Pe1U8 reserved_0f4[2];
    Pe1U8 bank_value_f6, bank_value_f7;
    unsigned short bank_value_f8, bank_value_fa;
    Pe1U8 unk_0fc[2];
    Pe1U8 transition_volume;        /* 0x0FE */
    Pe1U8 unk_0ff;
    Pe1U32 pe_image_base_lba;        /* 0x100: g_PeImageBaseLba */
    Pe1U8 draw_prim_b[0x10];         /* 0x104 */
    Pe1U8 draw_prim_c[8];            /* 0x114 */
    void *scene_object_model;        /* 0x11C */
    Pe1U8 draw_prim_c_tail[4];
    void *bank_asset_table;           /* 0x124: resolved bank asset table */
    Pe1U32 bank_work_base;           /* 0x128 */
    Pe1U32 bank_work_end;            /* 0x12C */
    Pe1U32 bank_work_far_end;        /* 0x130: voice bank base + 0x2800 */
    void *scene_object_tables[3];    /* 0x134 */
    Pe1U8 unk_140[0x0C];
    void *bank_asset_source;         /* 0x14C */
    Pe1U32 voice_bank_base;          /* 0x150 */
    Pe1U32 voice_bank_base_1400;     /* 0x154 */
    union SceneAssetView *entity_texture_blob; /* 0x158: voice bank base + 0x2800 */
    void *scene_object_work;         /* 0x15C */
    Pe1U8 unk_160[8];
    union SceneAssetView *texture_load_scratch; /* 0x168: second room read */
    Pe1U8 unk_16c[0xC];
    unsigned short *save_background_source;      /* 0x178 */
    unsigned short *save_background_destination; /* 0x17C */
    Pe1U8 unk_180[8];
    struct PmSlotBanks *scene_process_slots; /* 0x188 */
    union SceneAssetView *loaded_scene_assets; /* 0x18C */
    Pe1U8 unk_190[4];
    void *scene_load_scratch;        /* 0x194: g_SceneLoadScratchBuffer */
    /* Reset bounds establish these arrays; individual element roles unknown. */
    void *bank_slots[10];            /* 0x198 */
    void *bank_rows[10][48];         /* 0x1C0 */
    Pe1U32 bank_reset_940[1];
    void *bank_reset_944[1];         /* 0x944: relocated task block */
    void *bank_reset_948[1];
    void *bank_reset_94c[1];
    void *bank_reset_950[2];
    Pe1U32 bank_reset_958[1];
} Pe1GameState;


#endif
