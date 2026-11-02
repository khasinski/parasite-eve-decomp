/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/entity_frame_update.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/render_buffers.h"
#include "pe1/render_lighting.h"
#include "pe1/render_matrix_copy.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/render_setup.h"
#include "pe1/render_tint.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s16 g_SkinAnchorY;

/* Per-frame model transforms: bone matrices from the matrix command list,
 * the skinned hit volumes and the projected vertex buffers. */

/* Builds the object's bone matrices from its matrix command list (-1 pushes
 * the current GTE matrix, -2 pops it, any other value is a part index) and
 * transforms the hit-sphere centres of the visible parts. */
void Render_TransformVertices(RenderObjectEntity *actor) {
    s32 *origin = (s32 *)0x1F800000;
    s32 *originY = (s32 *)0x1F800004;
    s32 *originZ = (s32 *)0x1F800008;
    RenderMatrix *stack = (RenderMatrix *)0x1F80000C;
    s8 *commands;
    RenderMatrix *out;
    RenderAnimationLookupEntry *centres;
    RenderMatrix *current;
    int mode;
    int i;

    mode = actor->animation_state;
    if (mode == 1) {
        Render_CopyFrameData(actor, actor->animation_source, actor->animation_id);
        current = &actor->model_matrix;
    } else if (mode == 3) {
        if (actor->flags_9C & 0x400) {
            actor->matrices[1].rotation[0][0] = actor->model_matrix.rotation[0][0];
            actor->matrices[1].rotation[0][1] = actor->model_matrix.rotation[0][1];
            actor->matrices[1].rotation[0][2] = actor->model_matrix.rotation[0][2];
            actor->matrices[1].rotation[1][0] = actor->model_matrix.rotation[1][0];
            actor->matrices[1].rotation[1][1] = actor->model_matrix.rotation[1][1];
            actor->matrices[1].rotation[1][2] = actor->model_matrix.rotation[1][2];
            actor->matrices[1].rotation[2][0] = actor->model_matrix.rotation[2][0];
            actor->matrices[1].rotation[2][1] = actor->model_matrix.rotation[2][1];
            actor->matrices[1].rotation[2][2] = actor->model_matrix.rotation[2][2];
            actor->matrices[1].translation[0] = actor->model_matrix.translation[0];
            actor->matrices[1].translation[1] = actor->model_matrix.translation[1];
            actor->matrices[1].translation[2] = actor->model_matrix.translation[2];
        } else {
            Render_CopyFrameDataDouble(actor, actor->animation_source, actor->animation_id);
        }
        return;
    } else {
        if (mode == 4) {
            gte_CompMatrix(&actor->animation_source->matrices[actor->animation_id],
                           &actor->model_matrix, &actor->model_matrix);
        }
        current = &actor->model_matrix;
    }

    *origin = 0;
    *originY = 0;
    out = actor->matrices;
    commands = actor->matrix_commands;
    centres = actor->animation_entries;
    gte_ldrotmatrix(current);
    gte_ldtransmatrix(current);
    for (i = 0; i < actor->header->matrix_command_bytes; i++, commands++) {
        int command = *commands;
        RenderMatrix *source;

        if (command == -1) {
            gte_ReadMatrix(stack);
            stack++;
            continue;
        }
        if (command == -2) {
            stack--;
            gte_ldrotmatrix(stack);
            gte_ldtransmatrix(stack);
            continue;
        }
        source = &actor->active_matrix[command];
        gte_ldclmv(source);
        gte_rtir();
        *originZ = actor->parts[command].translation_z;
        gte_stclmv(out);
        gte_ldclmv((u16 *)source + 1);
        gte_rtir();
        gte_stclmv((s16 *)out + 1);
        gte_ldclmv((u16 *)source + 2);
        gte_rtir();
        gte_stclmv((s16 *)out + 2);
        if (*commands == 0)
            gte_ldlv0(source->translation);
        else
            gte_ldlv0(origin);
        gte_rt();
        gte_stlvnl(out->translation);
        gte_ldrotmatrix(out);
        gte_ldtransmatrix(out);
        out++;
        if (actor->parts[*commands].visible == 1 &&
            actor->bounds_vertices[*commands * 2 + 1].pad >= 0) {
            gte_ldv0(&actor->bounds_vertices[*commands * 2]);
            gte_rt();
            gte_stsv(centres);
            centres++;
        }
    }
}

/* Places a skinned object's hit volumes and projects its origin and target
 * point; headerless objects take their transforms from their owner. */
void Render_TransformSkinnedVertices(RenderObjectEntity *actor, u32 *view) {
    u16 *offset = (u16 *)0x1F800020;
    GteMatrix *scratch = (GteMatrix *)0x1F800000;
    RenderObjectHeader *header = actor->header;

    if (!header) {
        Render_SetupBoneTransforms(actor, (s32 *)view);
        return;
    }
    gte_ldrotmatrix(&actor->matrices[header->anchor_matrix_index]);
    gte_ldtransmatrix(&actor->matrices[header->anchor_matrix_index]);
    g_SkinAnchorY = header->anchor_y;
    gte_ldv0(g_SkinAnchorVector);
    gte_rt();
    gte_stsv(&actor->hit_cylinder.value0);
    gte_ldrotmatrix(actor->matrices);
    gte_ldtransmatrix(actor->matrices);
    offset[0] = ((u16 *)actor->model_section14)[0] + (u16)actor->hit_body.value0;
    offset[1] = ((u16 *)actor->model_section14)[1] + (u16)actor->hit_body.value1;
    offset[2] = ((u16 *)actor->model_section14)[2] + (u16)actor->hit_body.value2;
    gte_ldv0(offset);
    gte_rt();
    gte_stsv(&actor->hit_body.value0);
    gte_ldrotmatrix(&actor->matrices[(s16)actor->table_index]);
    gte_ldtransmatrix(&actor->matrices[(s16)actor->table_index]);
    gte_ldv0(&actor->bounds_vertices[(s16)actor->table_index * 2]);
    gte_rt();
    gte_stsv(&actor->target_x);
    gte_CompMatrixMac(view, actor->matrices, scratch);
    gte_ldrotmatrix(scratch);
    gte_ldtransmatrix(scratch);
    gte_ldv0(actor->projection_origin);
    gte_rtps();
    gte_stsxy2(&actor->projected_x);
    gte_CompMatrixMac(view, &actor->matrices[(s16)actor->table_index], scratch);
    gte_ldrotmatrix(scratch);
    gte_ldtransmatrix(scratch);
    gte_ldv0(&actor->bounds_vertices[(s16)actor->table_index * 2]);
    gte_rtps();
    gte_stsxy2(&actor->projected_target_x);
}

/* Projects every vertex triple of the object's visible parts through the
 * view-composed part matrix into the screen and depth buffers. */
void Render_TransformMorphVertices(RenderObjectEntity *entity, u32 *view) {
    GteMatrix *scratch = (GteMatrix *)0x1F800000;
    RenderMatrix *matrix = entity->matrices;
    int i;

    for (i = 0; i < entity->header->part_count; i++) {
        RenderObjectPart *part = &entity->parts[i];

        gte_CompMatrixMac(view, matrix, scratch);
        gte_ldrotmatrix(scratch);
        gte_ldtransmatrix(scratch);
        matrix++;
        if (part->visible == 1) {
            RenderVec3s *vertices = &entity->vertices[part->vertex_start];
            u32 *screen = &g_RenderClutLookupTable[part->vertex_start];
            u32 *depth = &g_RenderVertexWorkWords[part->vertex_start];
            int j;

            for (j = 0; j < part->vertex_count; ) {
                gte_ldv3c(vertices);
                gte_rtpt_padded();
                vertices += 3;
                j += 3;
                screen += 3;
                depth += 3;
                gte_stsxy3c(screen);
                gte_stsz3c(depth);
            }
        }
    }
}

void Render_DrawEntity(RenderObjectEntity *object, void *viewMatrix) {
    u16 flags;
    if (object->header == 0 || object->draw_count == 0) return;
    if (object->flags_9C & 0x800) {
        Render_SetObjectTexturePage(object, 0);
        Render_SetEntityBlendMode(object, 0);
        object->flags_9C &= ~0x800;
    }
    if (object->flags_9C & 0x10)
        Render_DrawObjectVariant(object, object->script_value9a, viewMatrix);
    if (object->variant_visible == 1)
        Render_DrawTexturedQuads(object);
    flags = object->flags_9C;
    if (flags & 0x20) {
        Render_BuildEntityMatrix(object, g_ActiveDrawSlot);
        object->reserved9f = g_ActiveDrawSlot == 0;
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
        Render_DrawObject(object, &g_RenderLightingState);
        Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                             object->script_param98, object->script_param99);
    } else if (flags & 1) {
        Render_DrawObject(object, &g_RenderLightingState);
        Render_UpdateClutTable(object, 0, g_ActiveDrawSlot);
    }
}

/* PSY-Q libgpu style ordering-table link through the 24-bit tag address. */
/* A packet seen as bytes and as the address the DMA chain links through. */
typedef union RenderPacketAddress {
    u8 *bytes;
    u32 word;
} RenderPacketAddress;

#define RENDER_TAG(p) ((RenderGpuTag *)(p))
#define RENDER_ADDPRIM(ot, packet) \
    (RENDER_TAG((packet).bytes)->address = RENDER_TAG(ot)->address, \
     RENDER_TAG(ot)->address = (packet).word)

/* NCLIP-tests each model face on the projected vertices and links the
 * faces with positive winding into the active ordering table at their
 * average depth; culled faces get a null link. Quads and triangles of both
 * packet classes are walked in turn through one shared descriptor cursor.
 * The room overlays carry a copy that keeps the negative winding instead. */
void Render_DrawTexturedQuads(RenderObjectEntity *entity) {
    int i;
    s32 *area = (s32 *)0x1F800000;
    u32 *xy = D_800B1104;
    RenderPrimitiveDescriptor *record = entity->primitive_descriptors;
    u8 *packets = entity->primitive_buffer;
    int slot = g_ActiveDrawSlot;
    u32 *ordering = D_800B08F8.ordering[slot];
    s32 *z = D_800A5E2C;
    u32 xy0, xy1, xy2;
    int a, b, c, d;
    RenderPacketAddress packet;

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
        packet.bytes = (u8 *)((RenderPacket34 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                RENDER_ADDPRIM(&ordering[depth], packet);
                ((RenderPacket34 *)packet.bytes)->sxy0 = xy0;
                ((RenderPacket34 *)packet.bytes)->sxy1 = xy1;
                ((RenderPacket34 *)packet.bytes)->sxy2 = xy2;
                ((RenderPacket34 *)packet.bytes)->sxy3 = xy[d];
            }
        } else {
            RENDER_TAG(packet.bytes)->address = 0;
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
        packet.bytes = (u8 *)((RenderPacket28 *)packets + slot);
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                RENDER_ADDPRIM(&ordering[depth], packet);
                ((RenderPacket28 *)packet.bytes)->sxy0 = xy0;
                ((RenderPacket28 *)packet.bytes)->sxy1 = xy1;
                ((RenderPacket28 *)packet.bytes)->sxy2 = xy2;
            }
        } else {
            RENDER_TAG(packet.bytes)->address = 0;
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
        packet.bytes = (u8 *)((RenderPacket24 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                RENDER_ADDPRIM(&ordering[depth], packet);
                ((RenderPacket24 *)packet.bytes)->sxy0 = xy0;
                ((RenderPacket24 *)packet.bytes)->sxy1 = xy1;
                ((RenderPacket24 *)packet.bytes)->sxy2 = xy2;
                ((RenderPacket24 *)packet.bytes)->sxy3 = xy[d];
            }
        } else {
            RENDER_TAG(packet.bytes)->address = 0;
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
        packet.bytes = (u8 *)((RenderPacket1C *)packets + slot);
        gte_stmac0(area);
        if (*area > 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                RENDER_ADDPRIM(&ordering[depth], packet);
                ((RenderPacket1C *)packet.bytes)->sxy0 = xy0;
                ((RenderPacket1C *)packet.bytes)->sxy1 = xy1;
                ((RenderPacket1C *)packet.bytes)->sxy2 = xy2;
            }
        } else {
            RENDER_TAG(packet.bytes)->address = 0;
        }
        packets += 2 * sizeof(RenderPacket1C);
        i++;
        record++;
    }
}

/* Callers pass a fourth argument the definition in anim/bone_rotations.c
 * does not declare (see entity_frame_update.h). */
int Anim_BuildRotationMatrices(RenderObjectEntity *object, void *action, int frame, int mode);

/* A GTE matrix copied as its eight 32-bit words. */
typedef union RenderMatrixWords {
    RenderMatrix matrix;
    s32 words[8];
} RenderMatrixWords;

void Render_BuildEntityMatrix(RenderObjectEntity *object, s16 bufferIndex)
{
    s32 saved[8];
    RenderMatrixWords *model;
    FieldActor *actor;
    s32 first;
    void *animation;
    RenderMatrixWords *player;

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
        Render_DrawObject(object, &g_RenderLightingState);
        Render_UpdateClutTable(object, 1, bufferIndex);
        model->words[0] = saved[0];
        model->words[1] = saved[1];
        model->words[2] = saved[2];
        model->words[3] = saved[3];
        model->words[4] = saved[4];
        model->words[5] = saved[5];
        model->words[6] = saved[6];
        model->words[7] = saved[7];
        actor = (FieldActor *)((char *)object - PE1_OFFSETOF(FieldActor, render_object));
        Anim_BuildRotationMatrices(object, actor->action_data, (s16)actor->anim.parts.integer, 1);
        Render_TransformVertices(object);
        return;
    }
    player = (RenderMatrixWords *)g_PlayerBattleEntity.matrices;
    player->words[0] = g_IdentityMatrixSource[0];
    player->words[1] = g_IdentityMatrixSource[1];
    player->words[2] = g_IdentityMatrixSource[2];
    player->words[3] = g_IdentityMatrixSource[3];
    player->words[4] = g_IdentityMatrixSource[4];
    player->words[5] = g_IdentityMatrixSource[5];
    player->words[6] = g_IdentityMatrixSource[6];
    player->words[7] = g_IdentityMatrixSource[7];
    Render_InitRoomPrimState(object);
    Render_DrawObject(object, &g_RenderLightingState);
    Render_UpdateClutTable(object, 1, bufferIndex);
}

/* Lights an object's visible parts: each part's light matrix is the view
 * light matrix times the part matrix, every vertex triple gets an NCCT colour
 * from the base colour, and the first coded vertex colour of the triple gets
 * a second NCCT into the alternate colour table. */
void Render_DrawObject(RenderObjectEntity *entity, union RenderLightingMatrix *view) {
    u32 *baseColour = (u32 *)0x1F800000;
    s16 *lightMatrix = (s16 *)0x1F800004;
    RenderMatrix *matrix;
    int i, j;

    if (!entity->header || !entity->draw_count)
        return;
    *baseColour = g_RenderFadeColour;
    matrix = entity->matrices;
    gte_SetBackColor(entity->shade, entity->shade, entity->shade);
    for (i = 0; i < entity->header->part_count; matrix++, i++) {
        RenderObjectPart *part = &entity->parts[i];
        int first;
        u32 *mainColours;
        u32 *altColours;
        RenderVec3s *vertex;

        if (part->visible != 1)
            continue;
        gte_MulMatrix0(view, matrix, lightMatrix);
        gte_SetLightMatrix(lightMatrix);
        first = part->vertex_start;
        mainColours = &g_RenderClutLookupTable[first];
        altColours = &g_RenderVertexWorkWords[first];
        vertex = &entity->vertices[first];
        for (j = 0; j < part->vertex_count; j += 3, mainColours += 3, altColours += 3) {
            RenderColor *colours;
            int k;

            gte_ldv3(&g_RenderNormalTable[vertex[0].pad], &g_RenderNormalTable[vertex[1].pad],
                     &g_RenderNormalTable[vertex[2].pad]);
            gte_ldrgb(baseColour);
            gte_ncct();
            vertex += 3;
            gte_strgb3(mainColours, mainColours + 1, mainColours + 2);
            k = first + j;
            colours = (RenderColor *)entity->vertex_colours;
            if (colours[k].code)
                gte_ldrgb(&colours[k]);
            else if (colours[k + 1].code)
                gte_ldrgb(&colours[k + 1]);
            else if (colours[k + 2].code)
                gte_ldrgb(&colours[k + 2]);
            else
                continue;
            gte_ncct();
            gte_strgb3(altColours, altColours + 1, altColours + 2);
        }
    }
}

#define NULL ((void *)0)

void Render_UpdateClutTable(RenderObjectEntity *arg0, s16 arg1, s16 arg2) {
    RenderObjectHeader *hdr;
    u32 *clutTbl;
    u32 *tpageTbl;
    RenderPrimitiveDescriptor *src;
    u8 *prim;
    u8 *p;
    s32 i;
    u8 cmd;

    hdr = arg0->header;
    if ((hdr != NULL) && (arg0->draw_count != 0)) {
        clutTbl = g_RenderClutLookupTable;
        tpageTbl = g_RenderVertexWorkWords;
        src = arg0->primitive_descriptors;
        prim = arg0->primitive_buffer;
        for (i = 0; i < arg0->header->packet34_count; src++) {
            p = prim + (arg2 * 0x34);
            if ((((RenderPacket34 *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket34 *)p)->values0.bytes.command;
                ((RenderPacket34 *)p)->values0.value = clutTbl[src->lookup_indices[0]];
                ((RenderPacket34 *)p)->value1 = clutTbl[src->lookup_indices[1]];
                ((RenderPacket34 *)p)->value2 = clutTbl[src->lookup_indices[2]];
                ((RenderPacket34 *)p)->value3 = clutTbl[src->lookup_indices[3]];
                ((RenderPacket34 *)p)->values0.bytes.command = cmd;
            }
            prim += 0x68;
            i++;
        }
        for (i = 0; i < arg0->header->packet28_count; src++) {
            p = prim + (arg2 * 0x28);
            if ((((RenderPacket28 *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket28 *)p)->values0.bytes.command;
                ((RenderPacket28 *)p)->values0.value = clutTbl[src->lookup_indices[0]];
                ((RenderPacket28 *)p)->value1 = clutTbl[src->lookup_indices[1]];
                ((RenderPacket28 *)p)->value2 = clutTbl[src->lookup_indices[2]];
                ((RenderPacket28 *)p)->values0.bytes.command = cmd;
            }
            prim += 0x50;
            i++;
        }
        for (i = 0; i < arg0->header->packet24_count; src++) {
            p = prim + (arg2 * 0x24);
            if ((((RenderPacket24 *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket24 *)p)->values0.bytes.command;
                ((RenderPacket24 *)p)->values0.value = tpageTbl[src->lookup_indices[0]];
                ((RenderPacket24 *)p)->value1 = tpageTbl[src->lookup_indices[1]];
                ((RenderPacket24 *)p)->value2 = tpageTbl[src->lookup_indices[2]];
                ((RenderPacket24 *)p)->value3 = tpageTbl[src->lookup_indices[3]];
                ((RenderPacket24 *)p)->values0.bytes.command = cmd;
            }
            prim += 0x48;
            i++;
        }
        for (i = 0; i < arg0->header->packet1c_count; src++) {
            p = prim + (arg2 * 0x1C);
            if ((((RenderPacket1C *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket1C *)p)->values0.bytes.command;
                ((RenderPacket1C *)p)->values0.value = tpageTbl[src->lookup_indices[0]];
                ((RenderPacket1C *)p)->value1 = tpageTbl[src->lookup_indices[1]];
                ((RenderPacket1C *)p)->value2 = tpageTbl[src->lookup_indices[2]];
                ((RenderPacket1C *)p)->values0.bytes.command = cmd;
            }
            prim += 0x38;
            i++;
        }
    }
}
#undef NULL

/* Colours the vertices of an object's visible parts that lie below a height
 * limit. GTE operations and their explicit nop hazard slots use individual
 * macros. */

#define Render_LoadObjectMatrix(matrix) \
    { \
        gte_ldrotmatrix((const GteMatrixWords *)(matrix)); \
        gte_ldtransmatrix((const GteMatrixWords *)(matrix)); \
    }

#define Render_TransformVertex(src, dst)                                                           \
    {                                                                                              \
        gte_ldv0(src);                                                                             \
        gte_rt();                                                                                  \
        gte_stsv(dst);                                                                             \
    }

void Render_DrawObjectAlt(RenderObjectEntity *entity, s16 limit, u8 red, u8 green, u8 blue) {
    RenderVec3s *scratch = (RenderVec3s *)0x1F800000;
    RenderMatrix *matrix;
    int i, j;

    if (!entity->header || !entity->draw_count)
        return;
    matrix = entity->matrices;
    for (i = 0; i < entity->header->part_count; i++, matrix++) {
        RenderObjectPart *part;
        s32 y;

        if (entity->parts[i].visible != 1)
            continue;
        Render_LoadObjectMatrix(matrix);
        Render_TransformVertex(&entity->bounds_vertices[i * 2], scratch);
        part = &entity->parts[i];
        y = scratch->y;
        if (limit < y || limit < y - entity->bounds_vertices[i * 2].pad ||
            limit < y + entity->bounds_vertices[i * 2].pad) {
            RenderVec3s *vertex = &entity->vertices[part->vertex_start];
            RenderColor *colour =
                (RenderColor *)&g_RenderClutLookupTable[part->vertex_start];

            for (j = 0; j < part->vertex_count; colour++) {
                /* Retail advances the counters while the GTE works. */
                gte_ldv0(vertex);
                gte_rt();
                j++;
                vertex++;
                gte_stsv(scratch);
                if (limit < scratch->y) {
                    colour->r = red;
                    colour->g = green;
                    colour->b = blue;
                }
            }
        }
    }
    Render_UpdateClutTable(entity, 0, g_ActiveDrawSlot);
}
#undef Render_LoadObjectMatrix
#undef Render_TransformVertex

#define Render_LoadObjectMatrix(matrix) \
    { \
        asm volatile("" : "=r"(matrix) : "0"(matrix)); \
        gte_ldrotmatrix((const GteMatrixWords *)(matrix)); \
        gte_ldtransmatrix((const GteMatrixWords *)(matrix)); \
    }

#define Render_TransformVertex(src, dst)                                                           \
    {                                                                                              \
        gte_ldv0(src);                                                                             \
        gte_rt();                                                                                  \
        gte_stsv(dst);                                                                             \
    }

void Render_DrawObjectVariant(RenderObjectEntity *input, s16 limit, s32 *projectionMatrix) {
    RenderObjectEntity *entity = input;
    s32 *projectMatrix = projectionMatrix;
    s16 savedLimit = limit;
    s32 changed;
    register volatile RenderVec3s *scratch asm("$24") = (RenderVec3s *)0x1F800000;
    u32 *projected;
    register s32 *matrix asm("$8");
    register s32 i asm("$11");
    register s32 offset asm("$25");
    register s32 threshold asm("$10");
    register RenderObjectPart *part asm("$9");
    register RenderObjectPart *probe asm("$4");
    s32 boundsOffset;
    RenderVec3s *bounds;
    s32 y;
    s32 radius;
    s32 sum;
    RenderVec3s *vertices;
    u32 *clut;
    s32 vertexIndex;
    register s32 limitShift asm("$2");
    changed = 0;
    __asm__("" : "=r"(entity) : "0"(entity), "r"(changed) : "$6");
    projected = (u32 *)0x1F800008;
    if (!entity->header || !entity->draw_count)
        return;
    matrix = (s32 *)entity->matrices;
    i = 0;
    if (changed < entity->header->part_count) {
        limitShift = limit << 16;
        threshold = limitShift >> 16;
        offset = 0;
        do {
            {
                register union {
                    RenderObjectPart *part;
                    u32 word;
                } base asm("$2");
                base.part = entity->parts;
                probe = (RenderObjectPart *)(offset + base.word);
            }
            if (probe->visible == 1) {
                part = probe;
                Render_LoadObjectMatrix(matrix);
                boundsOffset = i * 16;
                bounds = &entity->bounds_vertices[i * 2];
                Render_TransformVertex(bounds, scratch);
                {
                    u16 yy = (u16)scratch->y;
                    y = (s16)yy;
                }
                if (threshold < y ||
                    (bounds = (RenderVec3s *)(boundsOffset + ((RenderPacketAddress *)&entity->bounds_vertices)->word),
                     radius = bounds->pad,
                     threshold < y - radius || threshold < (sum = y + radius))) {
                    scratch->y = savedLimit;
                    Render_LoadObjectMatrix(projectMatrix);
                    gte_lwc2_0_0(scratch);
                    gte_lwc2_1_4(scratch);
                    gte_cop2_hazard_slot();
                    gte_cop2_hazard_slot();
                    gte_rtps_command();
                    gte_stsxy2(projected);
                    vertexIndex = 0;
                    {
                        s32 first = part->vertex_start;
                        RenderVec3s *base = entity->vertices;
                        vertices = &base[first];
                        clut = &D_800B1100[first];
                    }
                    Render_LoadObjectMatrix(matrix);
                    for (; vertexIndex < part->vertex_count; vertices++) {
                        gte_lwc2_0_0(vertices);
                        gte_lwc2_1_4(vertices);
                        gte_cop2_hazard_slot();
                        gte_cop2_hazard_slot();
                        gte_mvmva_rotation_v0_translation_sf12();
                        vertexIndex++;
                        clut++;
                        gte_stsv(scratch);
                        if (threshold < scratch->y) {
                            *clut = *projected;
                            changed++;
                        }
                    }
                }
            }
            offset += sizeof(RenderObjectPart);
            i++;
            matrix += 8;
        } while (i < entity->header->part_count);
    }
    if (changed == entity->header->visible_part_count || (entity->flags_9C & 0x200))
        entity->variant_visible = 0;
    else
        entity->variant_visible = 1;
}

void Anim_SetInterpRate(RenderObjectEntity *obj, int arg1) {
    short value = arg1;
    int divisor;

    if ((short)arg1 == 0) {
        value = 1;
    }
    divisor = 0x80 / value;

    obj->fade_duration = value;
    obj->fade_red_step = divisor;
    obj->fade_green_step = divisor;
    obj->fade_blue_step = divisor;
}

/* Adjacent object updates use the same animation and colour state. */

u32 g_RenderFadeColour = 0x808080;
int Render_TickObject(RenderObjectEntity *object) {
    int state;
    u8 r, g, b;
    if (!object->header || !object->draw_count) return 1;
    state = object->fade_remaining;
    if (!state) {
        object->fade_remaining = -1;
        Render_SetObjectTexturePage(object, 0);
        Render_InitRoomPrimState(object);
        Render_SetEntityBlendMode(object, 0);
        object->flags_9C |= 0x820;
        return 1;
    }
    if (state < 0) {
        object->variant_visible = 1;
        object->flags_9C &= ~0x200;
        Render_SetObjectTexturePage(object, 1);
        Render_SetEntityBlendMode(object, 1);
        r = object->primitive_red;
        g = object->primitive_green;
        b = object->primitive_blue;
        object->fade_red = 0;
        object->fade_green = 0;
        object->fade_blue = 0;
        Render_FadeEntityColor(object, 0, 0, 0);
        object->primitive_red = r;
        object->primitive_green = g;
        object->primitive_blue = b;
        object->fade_remaining = object->fade_duration;
    } else if (state == object->fade_duration - 1) {
        Render_SetObjectTexturePage(object, 1);
        Render_SetEntityBlendMode(object, 1);
    }
    g_RenderFadeColour = (object->fade_blue << 16) | (object->fade_green << 8) | object->fade_red;
    Render_DrawObject(object, &g_RenderLightingState);
    if (!(object->flags_9C & 8)) Render_UpdateClutTable(object, 0, (s16)g_ActiveDrawSlot);
    g_RenderFadeColour = 0x808080;
    object->fade_red += object->fade_red_step;
    object->fade_green += object->fade_green_step;
    object->fade_blue += object->fade_blue_step;
    object->fade_remaining--;
    return 0;
}

int Render_ColorEntity(RenderObjectEntity *object) {
    int state;
    if (!object->header || !object->draw_count) return 1;
    state = object->fade_remaining;
    if (!state) {
        object->fade_remaining = -1;
        g_RenderFadeColour = 0x808080;
        return 1;
    }
    if (state == 1) {
        object->variant_visible = 0;
    } else if (state < 0) {
        object->fade_remaining = object->fade_duration;
        Render_SetEntityBlendMode(object, 1);
        Render_SetObjectTexturePage(object, 1);
        if (object->primitive_red > 128) object->primitive_red = 128;
        if (object->primitive_green > 128) object->primitive_green = 128;
        if (object->primitive_blue > 128) object->primitive_blue = 128;
        object->fade_red_step = object->primitive_red / object->fade_duration;
        object->fade_green_step = object->primitive_green / object->fade_duration;
        object->fade_blue_step = object->primitive_blue / object->fade_duration;
        object->fade_red = object->primitive_red;
        object->fade_green = object->primitive_green;
        object->fade_blue = object->primitive_blue;
    } else if (state == object->fade_duration - 1) {
        Render_SetEntityBlendMode(object, 1);
        Render_SetObjectTexturePage(object, 1);
    } else {
        object->fade_red -= object->fade_red_step;
        if (object->fade_red > 128) object->fade_red = 0;
        object->fade_green -= object->fade_green_step;
        if (object->fade_green > 128) object->fade_green = 0;
        object->fade_blue -= object->fade_blue_step;
        if (object->fade_blue > 128) object->fade_blue = 0;
    }
    g_RenderFadeColour = (object->fade_blue << 16) | (object->fade_green << 8) | object->fade_red;
    Render_DrawObject(object, &g_RenderLightingState);
    if (!(object->flags_9C & 8)) Render_UpdateClutTable(object, 0, (s16)g_ActiveDrawSlot);
    g_RenderFadeColour = 0x808080;
    object->fade_remaining--;
    return 0;
}

/* Object fade and blend commands update the same colour packet state. */

void Render_FadeEntityColor(RenderObjectEntity *object, int r, int g, int b) {
    u8 *prim;
    u32 color;
    int i;
    int slot = g_ActiveDrawSlot;
    u8 opcode;

    if (!object->header || !object->draw_count) {
        return;
    }
    color = ((b & 255) << 16) | ((g & 255) << 8) | (r & 255);
    object->primitive_red = r;
    object->primitive_green = g;
    object->primitive_blue = b;
    prim = object->primitive_buffer;
    for (i = -1; ++i < object->header->packet34_count;
         prim += 2 * sizeof(RenderPacket34)) {
        RenderPacket34 *packet = (RenderPacket34 *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->value3 = color;
        packet->values0.bytes.command = opcode;
    }
    for (i = -1; ++i < object->header->packet28_count;
         prim += 2 * sizeof(RenderPacket28)) {
        RenderPacket28 *packet = (RenderPacket28 *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->values0.bytes.command = opcode;
    }
    for (i = -1; ++i < object->header->packet24_count;
         prim += 2 * sizeof(RenderPacket24)) {
        RenderPacket24 *packet = (RenderPacket24 *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->value3 = color;
        packet->values0.bytes.command = opcode;
    }
    for (i = -1; ++i < object->header->packet1c_count;
         prim += 2 * sizeof(RenderPacket1C)) {
        RenderPacket1C *packet = (RenderPacket1C *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->values0.bytes.command = opcode;
    }
}

static inline int primitiveBlendMode(int kind, int forcedKind, s16 mode) {
    if (kind == forcedKind) {
        return 1;
    }
    return mode;
}

static inline void setPrimitiveBlend(u8 *command, int enabled) {
    if (enabled) {
        *command |= 2;
    } else {
        *command &= ~2;
    }
}

void Render_SetEntityBlendMode(RenderObjectEntity *object, int mode) {
    int slot = g_ActiveDrawSlot;
    s16 savedMode = mode;
    int i;
    u8 *prim;
    RenderPrimitiveDescriptor *descriptor;

    if (!object->header || !object->draw_count) {
        return;
    }
    descriptor = object->primitive_descriptors;
    prim = object->primitive_buffer;
    for (i = 0; i < object->header->packet34_count;
         prim += 2 * sizeof(RenderPacket34), ++i, ++descriptor) {
        RenderPacket34 *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 11, savedMode);
        packet = (RenderPacket34 *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
    for (i = 0; i < object->header->packet28_count;
         prim += 2 * sizeof(RenderPacket28), ++i, ++descriptor) {
        RenderPacket28 *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 16, savedMode);
        packet = (RenderPacket28 *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
    for (i = 0; i < object->header->packet24_count;
         prim += 2 * sizeof(RenderPacket24), ++i, ++descriptor) {
        RenderPacket24 *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 21, savedMode);
        packet = (RenderPacket24 *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
    for (i = 0; i < object->header->packet1c_count;
         prim += 2 * sizeof(RenderPacket1C), ++i, ++descriptor) {
        RenderPacket1C *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 26, savedMode);
        packet = (RenderPacket1C *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
}

#define NULL ((void *)0)

/* Texture-page transition deltas, indexed by old and requested page. */

/* Moves every packet of the object to texture page `page`: the current page
 * is read from the first packet and the per-page delta added to all. */
void Render_SetObjectTexturePage(RenderObjectEntity *obj, s32 page) {
    int slot = g_ActiveDrawSlot;
    int current = 0;
    RenderPrimCursor prim;
    int delta;
    int i;

    if (obj->header == NULL || obj->draw_count == 0)
        return;
    prim.bytes = obj->primitive_buffer;
    if (obj->header->packet34_count != 0)
        current = (prim.p34[slot].page_bits & 0x7F) >> 5;
    else if (obj->header->packet28_count != 0)
        current = (prim.p28[slot].page_bits & 0x7F) >> 5;
    if ((s16)page == current)
        return;
    delta = g_RenderTexturePageDeltas[current][(s16)page];
    for (i = 0; i < obj->header->packet34_count; i++) {
        RenderPacket34 *packet = &prim.p34[slot];

        packet->page_bits += delta;
        prim.p34 += 2;
    }
    for (i = 0; i < obj->header->packet28_count; i++) {
        RenderPacket28 *packet = &prim.p28[slot];

        packet->page_bits += delta;
        prim.p28 += 2;
    }
}

/* Parses a model block into the render object: section pointers, the
 * double-buffered GPU packets (command codes and texture coordinates), the
 * animated part lookup entries and one identity matrix per part. */
int Render_SetupEntityPrims(RenderObjectEntity *obj, RenderObjectHeader *model,
                            u8 *packetBuffer, s16 x, s16 y, s16 unused,
                            s16 paletteRow, s16 initCount, s8 **textureOut,
                            int setup)
{
    RenderModelCursor src;
    RenderPrimitiveDescriptor *desc;
    RenderPrimCursor prim;
    RenderModelCursor cursor;
    RenderObjectHeader *header;
    RenderAnimationLookupEntry *entry;
    RenderMatrix *matrix;
    int bytes, words;
    s16 skipped;
    s16 i, j;

    cursor.header = model;
    obj->header = model;
    header = model;
    i = 0;
    skipped = 0;
    cursor.header++;
    obj->parts = cursor.parts;
    cursor.parts += model->part_count;
    obj->vertices = cursor.vectors;
    cursor.vectors += header->vertex_count;
    obj->vertex_colours = cursor.colours;
    cursor.colours += header->vertex_count;
    obj->primitive_descriptors = cursor.descriptors;
    prim.bytes = packetBuffer;
    obj->primitive_buffer = packetBuffer;
    desc = cursor.descriptors;
    obj->draw_count = setup;

    if (setup) {
        for (; i < header->packet34_count; i++, desc++) {
            prim.head->length = 12;
            prim.head->colour.bytes.command = 0x3C;
            if (desc->kind == 0x0B)
                prim.head->colour.bytes.command = 0x3E;
            prim.p34++;
            prim.head->length = 12;
            prim.head->colour.bytes.command = 0x3C;
            if (desc->kind == 0x0B)
                prim.head->colour.bytes.command = 0x3E;
            prim.p34++;
        }
        for (i = 0; i < header->packet28_count; i++, desc++) {
            prim.head->length = 9;
            prim.head->colour.bytes.command = 0x34;
            if (desc->kind == 0x10)
                prim.head->colour.bytes.command = 0x36;
            prim.p28++;
            prim.head->length = 9;
            prim.head->colour.bytes.command = 0x34;
            if (desc->kind == 0x10)
                prim.head->colour.bytes.command = 0x36;
            prim.p28++;
        }
        for (i = 0; i < header->packet24_count; i++, desc++) {
            prim.head->length = 8;
            prim.head->colour.bytes.command = 0x38;
            if (desc->kind == 0x15)
                prim.head->colour.bytes.command = 0x3A;
            prim.p24++;
            prim.head->length = 8;
            prim.head->colour.bytes.command = 0x38;
            if (desc->kind == 0x15)
                prim.head->colour.bytes.command = 0x3A;
            prim.p24++;
        }
        for (i = 0; i < header->packet1c_count; i++, desc++) {
            prim.head->length = 6;
            prim.head->colour.bytes.command = 0x30;
            if (desc->kind == 0x1A)
                prim.head->colour.bytes.command = 0x32;
            prim.p1c++;
            prim.head->length = 6;
            prim.head->colour.bytes.command = 0x30;
            if (desc->kind == 0x1A)
                prim.head->colour.bytes.command = 0x32;
            prim.p1c++;
        }
    } else {
        desc += header->packet34_count + header->packet28_count
              + header->packet24_count + header->packet1c_count;
    }
    cursor.descriptors = desc;

    obj->model_section14 = cursor.vectors;
    cursor.vectors += 2;
    obj->bounds_vertices = cursor.vectors;
    cursor.vectors += header->part_count * 2;
    obj->projection_origin = cursor.vectors;
    cursor.vectors++;
    obj->matrix_commands = cursor.commands;
    /* Matrix command block, rounded up to whole words. */
    bytes = header->matrix_command_bytes;
    words = (u16)bytes;
    words >>= 2;
    if ((bytes & 3) > 0) {
        bytes = words;
        bytes++;
        bytes *= 4;
    } else {
        bytes = words;
        bytes *= 4;
    }
    cursor.commands += bytes;

    if (setup) {
        RenderPrimCursor out;

        src = cursor;
        *textureOut = src.commands;
        out.bytes = obj->primitive_buffer;
        for (i = 0; i < header->packet34_count; i++) {
            src.quad = cursor.quad;

            for (j = 0; j < 2; j++) {
                out.p34->u0 = src.quad->u0;
                out.p34->v0 = src.quad->v0;
                out.p34->u1 = src.quad->u1;
                out.p34->v1 = src.quad->v1;
                out.p34->u2 = src.quad->u2;
                out.p34->v2 = src.quad->v2;
                out.p34->u3 = src.quad->u3;
                out.p34->v3 = src.quad->v3;
                out.p34->clut = src.quad->clut;
                out.p34->page_bits = src.quad->page_bits;
                out.p34++;
            }
            cursor.quad++;
        }
        for (i = 0; i < header->packet28_count; i++) {
            src.tri = cursor.tri;

            for (j = 0; j < 2; j++) {
                out.p28->u0 = src.tri->u0;
                out.p28->v0 = src.tri->v0;
                out.p28->u1 = src.tri->u1;
                out.p28->v1 = src.tri->v1;
                out.p28->u2 = src.tri->u2;
                out.p28->v2 = src.tri->v2;
                out.p28->clut = src.tri->clut;
                out.p28->page_bits = src.tri->page_bits;
                out.p28++;
            }
            cursor.tri++;
        }
        for (i = 0; i < header->packet24_count; i++) {
        }
        for (i = 0; i < header->packet1c_count; i++) {
        }
        if (initCount > 0)
            Render_InitPrimBlock(obj, x, y, unused, paletteRow);
    }

    {
        RenderVec3s *section = obj->model_section14;

        obj->hit_cylinder.radius = section->pad;
    }
    obj->hit_cylinder.bounds_value1 = obj->projection_origin->pad;
    obj->animation_entries = (RenderAnimationLookupEntry *)prim.head;
    entry = (RenderAnimationLookupEntry *)prim.head;
    for (i = 0; i < obj->header->part_count; i++) {
        if (obj->parts[i].visible == 1) {
            RenderVec3s *bounds = &obj->bounds_vertices[i * 2];

            if (bounds[1].pad >= 0) {
                entry->radius = bounds[0].pad;
                entry->bounds_value1 = bounds[1].pad;
                entry->animation_id = i;
                entry++;
            }
        } else {
            skipped++;
        }
    }

    obj->matrices = (RenderMatrix *)entry;
    matrix = (RenderMatrix *)entry;
    for (i = 0; i < obj->header->part_count;) {
        i++;
        matrix->rotation[0][0] = 0x1000;
        matrix->rotation[0][1] = 0;
        matrix->rotation[0][2] = 0;
        matrix->rotation[1][0] = 0;
        matrix->rotation[1][1] = 0x1000;
        matrix->rotation[1][2] = 0;
        matrix->rotation[2][0] = 0;
        matrix->rotation[2][1] = 0;
        matrix->rotation[2][2] = 0x1000;
        matrix->translation[0] = 0;
        matrix->translation[1] = 0;
        matrix->translation[2] = 0;
        matrix++;
    }

    obj->header->shadow_radius = 0;
    obj->table_value2c = 0;
    obj->table_value2e = 0;
    obj->table_value30 = 0;
    obj->table_index = 1;
    obj->table_value2c = 0;
    obj->table_value2e = 0;
    obj->table_value30 = 0;
    RotMatrix((GteShortVector *)&obj->table_value2c, (GteMatrix *)&obj->model_matrix);
    obj->fade_remaining = -1;
    Anim_SetInterpRate(obj, 0x32);
    obj->primitive_red = 0x80;
    obj->primitive_green = 0x80;
    obj->primitive_blue = 0x80;
    obj->flags_9C = 0;
    obj->variant_visible = 1;
    obj->reserved9f = g_ActiveDrawSlot;
    obj->animation_state = 0;
    obj->animation_source = 0;
    obj->animation_id = 0;
    obj->header->visible_part_count = obj->header->vertex_count - skipped;
    obj->hit_cylinder.animation_id = obj->projection_origin->y + (obj->hit_cylinder.bounds_value1 >> 4);
    for (i = 0; i < 2; i++) {
        obj->rotation_overrides[i].matrix_index = 0;
        obj->rotation_overrides[i].flags = 0;
    }
    obj->animation_data = 0;
    return 1;
}

int Render_ReturnTrue(void *unused);

int Render_ReturnTrue(void *unused) {
    return 1;
}

/* Draws an object with an optional animation frame applied on top of its
 * saved model matrix, then refreshes the packets of both draw buffers. */
void Render_DrawWithAnim(RenderObjectEntity *obj, void *animation, s16 frame,
                         union RenderLightingMatrix *light) {
    if (animation != 0) {
        obj->animation_data = animation;
        Render_CopyMatrixBlock(&obj->model_matrix, (RenderMatrix *)g_RenderClutLookupTable, 1);
        Render_CopyMatrixBlock((RenderMatrix *)g_IdentityMatrixSource, &obj->model_matrix, 1);
        Anim_BuildRotationMatrices(obj, animation, frame, 0);
    }
    Render_TransformVertices(obj);
    Render_CopyMatrixBlock((RenderMatrix *)g_RenderClutLookupTable, &obj->model_matrix, 1);
    Render_DrawObject(obj, light);
    Render_UpdateClutTable(obj, 1, g_ActiveDrawSlot);
    g_ActiveDrawSlot ^= 1;
    Render_UpdateClutTable(obj, 1, g_ActiveDrawSlot);
    g_ActiveDrawSlot ^= 1;
}

/* Equivalent branches preserve the stock compiler\'s register allocation.
 * No packet operation depends on which of these identical branches is taken. */

void Render_InitPrimBlock(RenderObjectEntity *object, s16 x, s16 y, int unused, unsigned int palette_row)
{
    unsigned int page;
    int page_offset;
    unsigned int scaled;
    u16 signed_page;
    unsigned int palette_offset;
    unsigned int row_copy;
    s16 half_page;
    int flag_copy;
    RenderPacket34 *quad;
    RenderPacket28 *tri;
    s16 i;
    s16 side;
    half_page = y == 128;
    page = ((((x >> 6) - 8) * 2) + (y >> 7)) + ((y >> 8) * 30);
    signed_page = page;
    row_copy = palette_row;
    quad = (RenderPacket34 *) object->primitive_buffer;
    if (x != 960)
    {
        page_offset = page >> 1;
        scaled = palette_row * 64;
    }
    else
    {
        if (quad->page_bits == 31)
        {
            return;
        }
        if (quad)
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        else
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        if (quad)
        {
            scaled = row_copy * 64;
        }
        else
        {
            scaled = row_copy * 64;
        }
    }
    signed_page = page;
    if (signed_page)
    {
        palette_offset = scaled - 0x7080;
    }
    else
    {
        palette_offset = scaled - 0x7080;
    }
    for (i = 0; i < object->header->packet34_count; i++)
    {
        flag_copy = half_page;
        for (side = 0; side < 2; side++, quad++)
        {
            if (flag_copy)
            {
                u8 v = quad->v0;
                if (v < 128)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset;
                }
                else
                    if (object)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
                else
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
            }
            else
            {
                quad->page_bits += page_offset;
            }
            quad->clut += palette_offset;
        }

    }

    tri = (RenderPacket28 *) quad;
    for (i = 0; i < object->header->packet28_count; i++)
    {
        for (side = 0; side < 2; side++, tri++)
        {
            if (half_page)
            {
                u8 v = tri->v0;
                if (v < 128)
                {
                    tri->v0 = v + 128;
                    tri->v1 += 128;
                    tri->v2 += 128;
                    scaled = palette_row * 64;
                    tri->page_bits += page_offset;
                }
                else
                {
                    tri->v0 = v + 128;
                    tri->v1 = tri->v1 + 128;
                    tri->v2 += 128;
                    tri->page_bits += page_offset + 1;
                }
            }
            else
            {
                tri->page_bits += page_offset;
            }
            tri->clut += palette_offset;
        }

    }

}

void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->model_matrix.rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->model_matrix.rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->model_matrix.rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->model_matrix.rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->model_matrix.rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->model_matrix.rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->model_matrix.rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->model_matrix.rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->model_matrix.rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->model_matrix.translation[0] = src->matrices[index].translation[0];
    dst->model_matrix.translation[1] = src->matrices[index].translation[1];
    dst->model_matrix.translation[2] = src->matrices[index].translation[2];
}

void Render_CopyFrameDataDouble(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->matrices[0].rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->matrices[0].rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->matrices[0].rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->matrices[0].rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->matrices[0].rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->matrices[0].rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->matrices[0].rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->matrices[0].rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->matrices[0].rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->matrices[0].translation[0] = src->matrices[index].translation[0];
    dst->matrices[0].translation[1] = src->matrices[index].translation[1];
    dst->matrices[0].translation[2] = src->matrices[index].translation[2];

    dst->matrices[1].rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->matrices[1].rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->matrices[1].rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->matrices[1].rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->matrices[1].rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->matrices[1].rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->matrices[1].rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->matrices[1].rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->matrices[1].rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->matrices[1].translation[0] = src->matrices[index].translation[0];
    dst->matrices[1].translation[1] = src->matrices[index].translation[1];
    dst->matrices[1].translation[2] = src->matrices[index].translation[2];
}

/* Bounds vectors come in pairs per table entry; the first one stores the
 * hit radius in its pad word. */
void Render_InitObjectFromTable(RenderObjectEntity *obj, RenderObjectEntity *owner, int index) {
    obj->table_index = index;
    obj->header = 0;
    obj->animation_source = owner;
    obj->hit_cylinder.radius = owner->bounds_vertices[(short)index * 2].pad;
    obj->table_value2c = owner->bounds_vertices[(short)index * 2].x;
    obj->table_value2e = owner->bounds_vertices[(short)index * 2].y;
    obj->table_value30 = owner->bounds_vertices[(short)index * 2].z + obj->hit_cylinder.radius;
}

void Render_Noop(void) {
}

int Render_ReturnZero(void) {
    return 0;
}

void Render_CopyMatrixBlock(RenderMatrix *src, RenderMatrix *dst, s16 count) {
    int i;

    for (i = 0; i < count; i++, src++, dst++) {
        dst->rotation[0][0] = src->rotation[0][0];
        dst->rotation[0][1] = src->rotation[0][1];
        dst->rotation[0][2] = src->rotation[0][2];
        dst->rotation[1][0] = src->rotation[1][0];
        dst->rotation[1][1] = src->rotation[1][1];
        dst->rotation[1][2] = src->rotation[1][2];
        dst->rotation[2][0] = src->rotation[2][0];
        dst->rotation[2][1] = src->rotation[2][1];
        dst->rotation[2][2] = src->rotation[2][2];
        dst->translation[0] = src->translation[0];
        dst->translation[1] = src->translation[1];
        dst->translation[2] = src->translation[2];
    }
}

void Render_SetObjectAnim(RenderObjectEntity *arg0, RenderObjectEntity *arg1, short arg2) {
    if (arg0->header->part_count == 2) {
        arg0->animation_source = arg1;
        arg0->animation_state = 3;
    } else {
        arg0->animation_source = arg1;
        arg0->animation_state = 1;
    }
    arg0->animation_id = arg2;
}

void Render_ClearObjectAnim(RenderObjectEntity *arg0) {
    arg0->animation_source = 0;
    if (arg0->header->part_count == 2) {
        arg0->animation_state = 2;
    } else {
        arg0->animation_state = 0;
    }
}

int Render_FindAnimEntry(RenderObjectEntity *obj, int id, s32 *out) {
    RenderAnimationLookupEntry *entry = obj->animation_entries;
    int i;

    for (i = 0; i < obj->header->animation_entry_count; i++, entry++) {
        if (entry->animation_id == (s16)id) {
            out[0] = entry->value0;
            out[1] = entry->value1;
            out[2] = entry->value2;
            return 1;
        }
    }
    return 0;
}

/* Matching debt: transfer and scratch pointers are pinned, with empty
 * address/memory constraints. Physical scratchpad addresses and pointer
 * casts remain. CPU loads, stores and vector packing are C; GTE transfers
 * and commands use individual macros. */
#define BoneLoadRotMatrix(matrix) gte_ldrotmatrix((const GteMatrixWords *)(matrix))

#define BoneLoadTrans(matrix) gte_ldtransmatrix((const GteMatrixWords *)(matrix))

#define BoneLoadFullMatrix(matrix)                                                                 \
    {                                                                                              \
        BoneLoadRotMatrix(matrix);                                                                 \
        BoneLoadTrans(matrix);                                                                     \
    }

void Render_SetupBoneTransforms(RenderObjectEntity *actor, s32 *view) {
    RenderMatrix *scratch = (RenderMatrix *)0x1F800000;
    RenderObjectEntity *source = actor->animation_source;
    int index = (s16)actor->table_index;
    RenderMatrix *matrix = &source->matrices[index];

    BoneLoadFullMatrix(matrix);
    gte_ldv0(&source->bounds_vertices[index * 2]);
    gte_rt();
    /* Headerless objects reuse the first override XYZ as a position output. */
    gte_stsv(&actor->rotation_overrides[0].x);
    gte_stsv(&actor->hit_cylinder.value0);
    gte_stsv(&actor->target_x);
    gte_CompMatrixMac(view, matrix, scratch);
    BoneLoadFullMatrix(scratch);
    /* In this mode the halfwords at +0x2C form the projection input vector. */
    gte_ldv0(&actor->table_value2c);
    gte_rtps();
    actor->hit_body.radius = source->hit_body.radius;
    gte_stsxy2(&actor->projected_target_x);
    actor->table_value2c += actor->hit_cylinder.radius;
    gte_ldv0(&actor->table_value2c);
    gte_rtps();
    actor->hit_body.value0 = source->hit_body.value0;
    actor->hit_body.value1 = source->hit_body.value1;
    actor->hit_body.value2 = source->hit_body.value2;
    gte_stsxy2(&actor->projected_x);
    actor->table_value2c -= actor->hit_cylinder.radius;
}

void Render_OffsetObjectTextureCoordinates(RenderObjectEntity *object, int du, int dv, int clutOffset) {
    u8 savedClut = clutOffset;
    RenderPacket34 *quad = (RenderPacket34 *)object->primitive_buffer;
    RenderPacket28 *tri;
    int i, j;
    u8 triClut;
    for (i = 0; i < object->header->packet34_count; i++) {
        for (j = 0; j < 2; j++, quad++) {
            quad->v0 += dv;
            quad->u0 += du;
            quad->v1 += dv;
            quad->u1 += du;
            quad->v2 += dv;
            quad->u2 += du;
            /* Retail leaves u3 unchanged. */
            quad->v3 += dv;
            quad->clut += (s8)clutOffset;
        }
    }
    triClut = savedClut;
    /* Byte-sized copies preserve the two signed CLUT conversions in plain C. */
    tri = (RenderPacket28 *)quad;
    for (i = 0; i < object->header->packet28_count; i++) {
        for (j = 0; j < 2; j++, tri++) {
            tri->v0 += dv;
            tri->u0 += du;
            tri->v1 += dv;
            tri->u1 += du;
            tri->v2 += dv;
            tri->u2 += du;
            tri->clut += (s8)triClut;
        }
    }
}

/* The counter word sits in .text right after this routine (main.yaml
 * render/Render_Counter) and the routine works in $a0/$a1, both signs of a
 * hand-written assembler original; the two pins reproduce those registers. */
void Render_IncrementCounter(void) {
    register int *ptr asm("$4");
    register int value asm("$5");

    ptr = g_RenderCounter;
    value = *ptr;
    value++;
    *ptr = value;
}
