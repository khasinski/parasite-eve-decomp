/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

extern u32 D_800B1640[];
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
                register RenderObjectPart *base asm("$2") = entity->parts;
                probe = (RenderObjectPart *)(offset + (u32)base);
            }
            if (probe->visible == 1) {
                part = probe;
                Render_LoadObjectMatrix(matrix);
                boundsOffset = i * 16;
                bounds = (RenderVec3s *)((u8 *)entity->bounds_vertices + boundsOffset);
                Render_TransformVertex(bounds, scratch);
                {
                    u16 yy = (u16)scratch->y;
                    y = (s16)yy;
                }
                if (threshold < y ||
                    (bounds = (RenderVec3s *)(boundsOffset + (u32)entity->bounds_vertices),
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
                        s32 vertexOffset = first * 8;
                        vertices = (RenderVec3s *)((u8 *)base + vertexOffset);
                        first <<= 2;
                        clut = (u32 *)((u32)D_800B1640 + first);
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

typedef struct {
    char pad[0x8D];
    u8 field8D;
    u8 field8E;
    u8 field8F;
    char pad90[3];
    u8 field93;
} Unk8003C5D8;

void Anim_SetInterpRate(Unk8003C5D8 *obj, int arg1) {
    short value = arg1;
    int divisor;

    if ((short)arg1 == 0) {
        value = 1;
    }
    divisor = 0x80 / (short)value;

    obj->field8D = value;
    obj->field8E = divisor;
    obj->field8F = divisor;
    obj->field93 = divisor;
}
