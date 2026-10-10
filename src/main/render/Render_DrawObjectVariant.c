/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_setup.h"

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

void Anim_SetInterpRate(RenderObjectEntity *obj, int arg1) {
    short value = arg1;
    int divisor;

    if ((short)arg1 == 0) {
        value = 1;
    }
    divisor = 0x80 / (short)value;

    obj->fade_duration = value;
    obj->fade_red_step = divisor;
    obj->fade_green_step = divisor;
    obj->fade_blue_step = divisor;
}

/* Adjacent object updates use the same animation and colour state. */

#include "pe1/render_lighting.h"
#include "pe1/render_prim.h"
u32 D_8009CDA0 = 0x808080;
int Render_TickObject(RenderObjectEntity *object) {
    int state;
    u8 r, g, b;
    if (!object->header || !object->draw_count) return 1;
    state = object->fade_remaining;
    if (!state) {
        object->fade_remaining = -1;
        Field_GetMapEntry(object, 0);
        Render_InitRoomPrimState(object);
        Render_SetEntityBlendMode(object, 0);
        object->flags_9C |= 0x820;
        return 1;
    }
    if (state < 0) {
        object->variant_visible = 1;
        object->flags_9C &= ~0x200;
        Field_GetMapEntry(object, 1);
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
        Field_GetMapEntry(object, 1);
        Render_SetEntityBlendMode(object, 1);
    }
    D_8009CDA0 = (object->fade_blue << 16) | (object->fade_green << 8) | object->fade_red;
    Render_DrawObject(object, &D_800BEA40);
    if (!(object->flags_9C & 8)) Render_UpdateClutTable(object, 0, (s16)D_8009CDDC);
    D_8009CDA0 = 0x808080;
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
        D_8009CDA0 = 0x808080;
        return 1;
    }
    if (state == 1) {
        object->variant_visible = 0;
    } else if (state < 0) {
        object->fade_remaining = object->fade_duration;
        Render_SetEntityBlendMode(object, 1);
        Field_GetMapEntry(object, 1);
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
        Field_GetMapEntry(object, 1);
    } else {
        object->fade_red -= object->fade_red_step;
        if (object->fade_red > 128) object->fade_red = 0;
        object->fade_green -= object->fade_green_step;
        if (object->fade_green > 128) object->fade_green = 0;
        object->fade_blue -= object->fade_blue_step;
        if (object->fade_blue > 128) object->fade_blue = 0;
    }
    D_8009CDA0 = (object->fade_blue << 16) | (object->fade_green << 8) | object->fade_red;
    Render_DrawObject(object, &D_800BEA40);
    if (!(object->flags_9C & 8)) Render_UpdateClutTable(object, 0, (s16)D_8009CDDC);
    D_8009CDA0 = 0x808080;
    object->fade_remaining--;
    return 0;
}

/* Object fade and blend commands update the same colour packet state. */

void Render_FadeEntityColor(RenderObjectEntity *object, int r, int g, int b) {
    u8 *prim;
    u32 color;
    int i;
    int slot = D_8009CDDC;
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
    int slot = D_8009CDDC;
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
