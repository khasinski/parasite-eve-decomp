#ifndef PE1_SCENE_ENTITY_TEXTURES_H
#define PE1_SCENE_ENTITY_TEXTURES_H

#include "common.h"
#include "pe1/scene_assets.h"
#include "pe1/field_actor.h"

/*
 * Scene_LoadEntityTextures: 14-state loader for the area entity bank.
 * States 1..3 stream and upload the per-day TIM set, 4..6 stream the
 * entity bank rows, 7 reads the selected CD stream, 11..13 rebuild the
 * scene render object from the loaded bank.
 */

/* Records of SceneAssetDirectory.reserved2c: offsets plus the CD read index. */
typedef struct SceneCdStreamRecord {
    u32 reserved;
    u32 offset; /* Low 24 bits: byte offset in the loaded blob. */
    u8 cdIndex;
    u8 trailing[3];
} SceneCdStreamRecord;

/* Byte view of a loaded scene container; records hold offsets from its base. */
typedef union SceneAssetView {
    SceneAssetBlob header;
    u8 bytes[1];
} SceneAssetView;

#define SCENE_ASSET_AT(view, offset) ((void *)&(view)->bytes[offset])

extern u8 g_SceneAreaType;
extern s8 D_800B0CE4;
extern u32 g_PeImageBaseLba;
extern FieldActor *g_PlayerEntity;
extern u16 D_800930D8[], D_800930DA[];
extern u8 D_800BEA40[];
extern u8 g_EntityRenderScratch[];

int Scene_UpdateBgDraw(void);
int CdRom_PollReady(void);
int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size);
int Render_SetupEntityPrims(void *object, void *model, void *work, int a3,
                            int a4, int a5, int a6, int a7, void *setup,
                            int a9);
int Render_InitRoomPrimState(void *object);
void Render_DrawWithAnim(void *object, int animation, int frame,
                         void *matrices, void *scratch);
int Scene_LoadEntityTextures(void);

#endif
