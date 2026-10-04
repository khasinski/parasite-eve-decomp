#include "pe1/render_setup.h"

/* Parses a model block into the render object: section pointers, the
 * double-buffered GPU packets (command codes and texture coordinates), the
 * animated part lookup entries and one identity matrix per part. */
int Render_SetupEntityPrims(RenderObjectEntity *obj, RenderObjectHeader *model,
                            RenderPrimCursor prims, s16 x, s16 y, s16 unused,
                            s16 paletteRow, s16 initCount, s8 **textureOut,
                            int setup)
{
    RenderQuadTexture *srcquad;
    RenderTriTexture *srctri;
    RenderPrimitiveDescriptor *desc;
    RenderPrimCursor prim;
    RenderModelCursor cursor;
    RenderObjectHeader *header;
    RenderAnimationLookupEntry *entry;
    RenderMatrix *matrix;
    int bytes, words;
    int skipped;
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
    prim = prims;
    obj->primitive_buffer = prims.bytes;
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
    bytes = header->matrix_command_bytes;
    words = (u16)bytes >> 2;
    if ((bytes & 3) > 0)
        bytes = (words + 1) * 4;
    else
        bytes = words * 4;
    cursor.commands += bytes;

    if (setup) {
        RenderPrimCursor out;

        *textureOut = cursor.commands;
        out.bytes = obj->primitive_buffer;
        for (i = 0; i < header->packet34_count; i++) {
            srcquad = cursor.quad;

            for (j = 0; j < 2; j++) {
                out.p34->u0 = srcquad->u0;
                out.p34->v0 = srcquad->v0;
                out.p34->u1 = srcquad->u1;
                out.p34->v1 = srcquad->v1;
                out.p34->u2 = srcquad->u2;
                out.p34->v2 = srcquad->v2;
                out.p34->u3 = srcquad->u3;
                out.p34->v3 = srcquad->v3;
                out.p34->clut = srcquad->clut;
                out.p34->page_bits = srcquad->page_bits;
                out.p34++;
            }
            cursor.quad++;
        }
        for (i = 0; i < header->packet28_count; i++) {
            srctri = cursor.tri;

            for (j = 0; j < 2; j++) {
                out.p28->u0 = srctri->u0;
                out.p28->v0 = srctri->v0;
                out.p28->u1 = srctri->u1;
                out.p28->v1 = srctri->v1;
                out.p28->u2 = srctri->u2;
                out.p28->v2 = srctri->v2;
                out.p28->clut = srctri->clut;
                out.p28->page_bits = srctri->page_bits;
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

        obj->table_value70 = section->pad;
    }
    obj->origin_value = obj->projection_origin->pad;
    obj->animation_entries = (RenderAnimationLookupEntry *)prim.head;
    entry = (RenderAnimationLookupEntry *)prim.head;
    for (i = 0; i < obj->header->part_count; i++) {
        if (obj->parts[i].visible == 1) {
            RenderVec3s *bounds = &obj->bounds_vertices[i * 2];

            if (bounds[1].pad >= 0) {
                entry->bounds_value0 = bounds[0].pad;
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
    obj->reserved9f = D_8009CDDC;
    obj->animation_state = 0;
    obj->animation_source = 0;
    obj->animation_id = 0;
    obj->header->visible_part_count = obj->header->vertex_count - skipped;
    obj->anchor_position.pad = obj->projection_origin->y + (obj->origin_value >> 4);
    for (i = 0; i < 2; i++) {
        obj->rotation_overrides[i].matrix_index = 0;
        obj->rotation_overrides[i].flags = 0;
    }
    obj->animation_data = 0;
    return 1;
}
