#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/render_lighting.h"
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_tint.h"
#include "pe1/field_actor.h"

void Render_DrawObjectVariant(RenderObjectEntity *, s16, void *);
void Render_DrawTexturedQuads(RenderObjectEntity *);
void Render_BuildEntityMatrix(RenderObjectEntity *, s16);
void Render_DrawObjectAlt(RenderObjectEntity *, s16, u8, u8, u8);

void Render_DrawEntity(RenderObjectEntity *object, void *viewMatrix) {
    u16 flags;
    if (object->header == 0 || object->draw_count == 0) return;
    if (object->flags_9C & 0x800) {
        Field_GetMapEntry(object, 0);
        Render_SetEntityBlendMode(object, 0);
        object->flags_9C &= ~0x800;
    }
    if (object->flags_9C & 0x10)
        Render_DrawObjectVariant(object, object->script_value9a, viewMatrix);
    if (object->variant_visible == 1)
        Render_DrawTexturedQuads(object);
    flags = object->flags_9C;
    if (flags & 0x20) {
        Render_BuildEntityMatrix(object, (s16)D_8009CDDC);
        object->reserved9f = (u32)D_8009CDDC < 1;
        object->flags_9C = (object->flags_9C & ~0x20) | 0x40;
    } else if (flags & 0x40) {
        Render_BuildEntityMatrix(object, object->reserved9f);
        object->flags_9C &= ~0x40;
    }
    flags = object->flags_9C;
    if (flags & 2) {
        if ((s16)Render_ColorEntity(object))
            object->flags_9C = (object->flags_9C & ~2) | 0x200;
        if (object->flags_9C & 8)
            Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                                 object->script_param98, object->script_param99);
    } else if (flags & 4) {
        if ((s16)Render_TickObject(object))
            object->flags_9C &= ~4;
        if (object->flags_9C & 8)
            Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                                 object->script_param98, object->script_param99);
    } else if (flags & 8) {
        Render_DrawObject(object, &D_800BEA40);
        Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                             object->script_param98, object->script_param99);
    } else if (flags & 1) {
        Render_DrawObject(object, &D_800BEA40);
        Render_UpdateClutTable(object, 0, (s16)D_8009CDDC);
    }
}

/* Projected screen XY and view depth of the transformed model vertices. */
extern u32 D_800B1644[];
extern s32 D_800A636C[];

/* PSY-Q libgpu style ordering-table link through the 24-bit tag address. */
#define RENDER_SETADDR(p, a) (((RenderGpuTag *)(p))->address = (u32)(a))
#define RENDER_GETADDR(p) (((RenderGpuTag *)(p))->address)
#define RENDER_ADDPRIM(ot, p) \
    RENDER_SETADDR(p, RENDER_GETADDR(ot)), RENDER_SETADDR(ot, p)

/* NCLIP-tests each model face on the projected vertices and links the
 * faces with positive winding into the active ordering table at their
 * average depth; culled faces get a null link. Quads and triangles of both
 * packet classes are walked in turn through one shared descriptor cursor.
 * The room overlays carry a copy that keeps the negative winding instead. */
void Render_DrawTexturedQuads(RenderObjectEntity *entity) {
    int i;
    s32 *area = (s32 *)0x1F800000;
    u32 *xy = D_800B1644;
    RenderPrimitiveDescriptor *record = entity->primitive_descriptors;
    u8 *packets = entity->primitive_buffer;
    int slot = D_8009CDDC;
    u32 *ordering = (u32 *)D_800B0E38.ordering[slot];
    s32 *z = D_800A636C;
    u32 xy0, xy1, xy2;
    int a, b, c, d;
    u8 *packet;

    i = 0;
    while (i < entity->header->packet34_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket34 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                RENDER_ADDPRIM(entry, packet);
                ((RenderPacket34 *)packet)->sxy0 = xy0;
                ((RenderPacket34 *)packet)->sxy1 = xy1;
                ((RenderPacket34 *)packet)->sxy2 = xy2;
                ((RenderPacket34 *)packet)->sxy3 = xy[d];
            }
        } else {
            RENDER_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket34);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet28_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket28 *)packets + slot);
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                RENDER_ADDPRIM(entry, packet);
                ((RenderPacket28 *)packet)->sxy0 = xy0;
                ((RenderPacket28 *)packet)->sxy1 = xy1;
                ((RenderPacket28 *)packet)->sxy2 = xy2;
            }
        } else {
            RENDER_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket28);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet24_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket24 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                RENDER_ADDPRIM(entry, packet);
                ((RenderPacket24 *)packet)->sxy0 = xy0;
                ((RenderPacket24 *)packet)->sxy1 = xy1;
                ((RenderPacket24 *)packet)->sxy2 = xy2;
                ((RenderPacket24 *)packet)->sxy3 = xy[d];
            }
        } else {
            RENDER_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket24);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet1c_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket1C *)packets + slot);
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                RENDER_ADDPRIM(entry, packet);
                ((RenderPacket1C *)packet)->sxy0 = xy0;
                ((RenderPacket1C *)packet)->sxy1 = xy1;
                ((RenderPacket1C *)packet)->sxy2 = xy2;
            }
        } else {
            RENDER_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket1C);
        i++;
        record++;
    }
}

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
