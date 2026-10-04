#ifndef PE1_ENTITY_FRAME_UPDATE_H
#define PE1_ENTITY_FRAME_UPDATE_H

/* Declarations used only by the per-frame entity driver Entity_FrameUpdate. */

#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead;
extern FieldActor *g_PlayerEntity;
/* Global game-state flags (bit 4 = menu open, 2 = battle, 0x100 =
 * player-only animation) and the word after them. */
typedef struct GameStateBlock {
    u32 flags;
    u32 reserved;
} GameStateBlock;
extern GameStateBlock g_GameStateBlock __asm__("D_8009D1A0");
extern u32 D_8009D1F4[];       /* pad trigger bits */
extern s16 D_8009D2A4[];       /* menu frame result */
extern u8 D_800A76D8[];        /* menu frame argument */
extern u32 D_800B0CD8[];       /* scene state flags */
extern u32 D_800B89F8[];       /* view matrix packet */
extern s16 D_800BCFFE[];       /* camera height (integer part) */

void Battle_Update(void);
void Inventory_OpenAyaItemList(unsigned int mode);
int Render_BeginSceneLoad(void);
int Menu_RunFrameWithArg(u8 *arg);
void func_80069660(void);
void func_8001A9F8(void);
void Entity_CopyParentPosition(void);
int Scene_IsNotBattleMode(void);
void Render_SetViewport(int *camera);
void Render_SetGteScreenOffset(void);
void Render_ResetGteScreenOffset(void);
int Render_InitRoomPrimState(void *object);
int Anim_BuildRotationMatrices(RenderObjectEntity *object, void *action, int frame, int mode);
void Render_TransformVertices(RenderObjectEntity *object);
void Render_TransformSkinnedVertices(RenderObjectEntity *object, u32 *view_matrix);
void Render_TransformMorphVertices(RenderObjectEntity *object, u32 *view_matrix);
void Render_DrawEntity(RenderObjectEntity *object, u32 *view_matrix);
int Render_DrawRoom(void *actor);
void Scene_LoadEntityTextures(void);
void func_80069594(void);
void Entity_AdvanceAnim(FieldActor *actor);
void Scene_UpdateEntityPositions(void);
void func_80012774(void);
void Entity_CollectGarbage(void);

#endif /* PE1_ENTITY_FRAME_UPDATE_H */
