#include "pe1/field_actor.h"
#include "pe1/render_lighting.h"

void Anim_BuildRotationMatrices(RenderObjectEntity *object, void *animation, int frame, int flags);
void Render_TransformVertices(RenderObjectEntity *object);

/* A GTE matrix copied as its eight 32-bit words. */
typedef union RenderMatrixWords {
    RenderMatrix matrix;
    s32 words[8];
} RenderMatrixWords;

extern s32 g_IdentityMatrixSource[8];
extern RenderObjectEntity g_PlayerBattleEntity;
extern s32 *g_PlayerMatrixPtr;

void Render_BuildEntityMatrix(RenderObjectEntity *object, s16 bufferIndex)
{
    s32 saved[8];
    RenderMatrixWords *model;
    FieldActor *actor;
    s32 first;
    void *animation;
    s32 *player;

    if (object != &g_PlayerBattleEntity) {
        model = (RenderMatrixWords *)&object->model_matrix;
        first = model->words[0];
        saved[1] = model->words[1];
        saved[2] = model->words[2];
        saved[3] = model->words[3];
        saved[4] = model->words[4];
        saved[5] = model->words[5];
        saved[6] = model->words[6];
        saved[7] = model->words[7];
        animation = object->animation_data;
        saved[0] = first;
        model->words[0] = g_IdentityMatrixSource[0];
        model->words[1] = g_IdentityMatrixSource[1];
        model->words[2] = g_IdentityMatrixSource[2];
        model->words[3] = g_IdentityMatrixSource[3];
        model->words[4] = g_IdentityMatrixSource[4];
        model->words[5] = g_IdentityMatrixSource[5];
        model->words[6] = g_IdentityMatrixSource[6];
        model->words[7] = g_IdentityMatrixSource[7];
        Anim_BuildRotationMatrices(object, animation, 0, 1);
        Render_TransformVertices(object);
        Render_InitRoomPrimState(object);
        Render_DrawObject(object, &D_800BEA40);
        Render_UpdateClutTable(object, 1, bufferIndex);
        model->words[0] = saved[0];
        model->words[1] = saved[1];
        model->words[2] = saved[2];
        model->words[3] = saved[3];
        model->words[4] = saved[4];
        model->words[5] = saved[5];
        model->words[6] = saved[6];
        model->words[7] = saved[7];
        actor = (FieldActor *)((char *)object - 0x1B4);
        Anim_BuildRotationMatrices(object, actor->action_data, (s16)actor->anim.parts.integer, 1);
        Render_TransformVertices(object);
        return;
    }
    player = g_PlayerMatrixPtr;
    player[0] = g_IdentityMatrixSource[0];
    player[1] = g_IdentityMatrixSource[1];
    player[2] = g_IdentityMatrixSource[2];
    player[3] = g_IdentityMatrixSource[3];
    player[4] = g_IdentityMatrixSource[4];
    player[5] = g_IdentityMatrixSource[5];
    player[6] = g_IdentityMatrixSource[6];
    player[7] = g_IdentityMatrixSource[7];
    Render_InitRoomPrimState(object);
    Render_DrawObject(object, &D_800BEA40);
    Render_UpdateClutTable(object, 1, bufferIndex);
}
