/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

/* Matching debt: register pins, empty barriers, volatile shade reads,
 * gotos, and an artificial 16-byte local frame reservation. */
extern RenderVec3s D_80091A58[];
extern u32 D_8009CDA0;

#define Draw_LoadRotation(m) gte_ldrotmatrix((const GteMatrixWords *)(m))
#define Draw_LoadLight(m)                                                                          \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        x = m[0];                                                                                  \
        y = m[1];                                                                                  \
        gte_ctc2_8(x);                                                                             \
        gte_ctc2_9(y);                                                                             \
        x = m[2];                                                                                  \
        y = m[3];                                                                                  \
        z = m[4];                                                                                  \
        gte_ctc2_10(x);                                                                            \
        gte_ctc2_11(y);                                                                            \
        gte_ctc2_12(z);                                                                            \
    }
/* Column loads/stores are C; retain the SDK transfer registers and addresses.
 * Address constraints remain for the second and third matrix columns. */
#define Draw_LoadAxis(src) \
    { \
        asm volatile("" : "=r"((src)) : "0"((src))); \
        gte_ldclmv((src)); \
        gte_rtir(); \
    }
#define Draw_StoreAxis(dst) \
    { \
        asm volatile("" : "=r"((dst)) : "0"((dst))); \
        gte_stclmv((dst)); \
        asm volatile("" : : : "memory"); \
    }
#define Draw_StoreColours(out)                                                                     \
    {                                                                                              \
        register u32 *out1 asm("$3") = out + 1;                                                    \
        u32 *out2 = out + 2;                                                    \
        gte_swc2_20_0(out);                                                                         \
        gte_swc2_21_0(out1);                                                                        \
        gte_swc2_22_0(out2);                                                                        \
    }
void Render_DrawObject(RenderObjectEntity *input, union RenderLightingMatrix *view) {
    RenderObjectEntity *entity = input;
    s32 *viewMatrix = (s32 *)view;
    u32 *baseColour = (u32 *)0x1F800000;
    register s16 *lightMatrix asm("$5") = (s16 *)0x1F800004;
    register RenderVec3s *normals asm("$16");
    register s32 offset asm("$17");
    s32 *matrix;
    register s32 partIndex asm("$25");
    register RenderObjectPart *part asm("$15");
    s32 vertexIndex;
    s32 first;
    u32 *mainColours;
    u32 *altColours;
    register s16 *normalIndex asm("$7");
    u32 frameReserve[4];
    register u8 shadeR asm("$20"), shadeG asm("$21"), shadeB asm("$22");
    if (!entity->header || !entity->draw_count)
        return;
    *baseColour = D_8009CDA0;
    matrix = (s32 *)entity->matrices;
    __asm__("" : "=r"(matrix) : "0"(matrix) : "memory");
    shadeR = *(volatile u8 *)&entity->shade;
    shadeG = *(volatile u8 *)&entity->shade;
    shadeB = *(volatile u8 *)&entity->shade;
    {
        register s32 r asm("$12") = shadeR << 4;
        register s32 g asm("$13") = shadeG << 4;
        register s32 b asm("$14") = shadeB << 4;
        gte_ctc2_13(r);
        gte_ctc2_14(g);
        gte_ctc2_15(b);
    }
    partIndex = 0;
    {
        s32 count = entity->header->part_count;
        if (count > 0) {
            normals = D_80091A58;
            offset = 0;
            do {
                {
                    register RenderObjectPart *parts asm("$2") = entity->parts;
                    part = (RenderObjectPart *)((u8 *)parts + offset);
                }
                __asm__("" : "=r"(part) : "0"(part));
                if (part->visible == 1) {
                    Draw_LoadRotation(viewMatrix);
                    {
                        gte_ldclmv((const u16 *)matrix);
                        gte_rtir();
                    }
                    {
                        gte_stclmv(lightMatrix);
                        asm volatile("" : : : "memory");
                    }
                    {
                        u16 *src = (u16 *)matrix + 1;
                        volatile s16 *dst;
                        Draw_LoadAxis(src);
                        dst = lightMatrix + 1;
                        Draw_StoreAxis(dst);
                    }
                    {
                        u16 *src = (u16 *)matrix + 2;
                        volatile s16 *dst;
                        Draw_LoadAxis(src);
                        dst = lightMatrix + 2;
                        Draw_StoreAxis(dst);
                    }
                    Draw_LoadLight(((s32 *)lightMatrix));
                    vertexIndex = 0;
                    first = part->vertex_start;
                    {
                        s32 byteOffset = first * 4;
                        register u32 *colourBase asm("$2") = D_800B1638;
                        mainColours = (u32 *)((u8 *)colourBase + byteOffset);
                        colourBase = D_800A6360;
                        altColours = (u32 *)((u8 *)colourBase + byteOffset);
                    }
                    {
                        register s32 vertexOffset asm("$4") = first * 8;
                        register RenderVec3s *vertices asm("$2") = entity->vertices;
                        s32 vertexCount;
                        __asm__("" : "=r"(part) : "0"(part), "r"(vertices));
                        vertexCount = part->vertex_count;
                        __asm__("" : "=r"(vertexCount) : "0"(vertexCount));
                        if (vertexCount > 0) {
                            vertices = (RenderVec3s *)((u8 *)vertices + vertexOffset);
                            normalIndex = &vertices[2].pad;
                            do {
                                {
                                    s32 a = normalIndex[-8];
                                    s32 b = normalIndex[-4];
                                    s32 c = normalIndex[0];
                                    register RenderVec3s *na asm("$4"), *nb,
                                        *nc;
                                    na = (RenderVec3s *)((a * 8) + (u32)normals);
                                    nb = (RenderVec3s *)((b * 8) + (u32)normals);
                                    nc = (RenderVec3s *)((c * 8) + (u32)normals);
                                    gte_lwc2_0_0(na);
                                    gte_lwc2_1_4(na);
                                    gte_lwc2_2_0(nb);
                                    gte_lwc2_3_4(nb);
                                    gte_lwc2_4_0(nc);
                                    gte_lwc2_5_4(nc);
                                }
                                gte_lwc2_6_0(baseColour);
                                gte_cop2_hazard_slot();
                                gte_cop2_hazard_slot();
                                gte_ncct_command();
                                normalIndex += 12;
                                Draw_StoreColours(mainColours);
                                {
                                    register s32 index asm("$2") = first + vertexIndex;
                                    u32 *colours = entity->vertex_colours;
                                    register s32 byteOffset asm("$4");
                                    u8 *colour;
                                    __asm__("" : "=r"(index) : "0"(index));
                                    byteOffset = index * 4;
                                    colour = (u8 *)(byteOffset + (u32)colours);
                                    if (colour[3]) {
                                        gte_lwc2_6_0(colour);
                                        goto shade_override;
                                    } else if (colour[7]) {
                                        s32 selectedOffset = byteOffset + 4;
                                        {
                                            register u32 *selected asm("$2") =
                                                (u32 *)((u8 *)colours + selectedOffset);
                                            gte_lwc2_6_0(selected);
                                        }
                                        goto shade_override;
                                    } else if (colour[11]) {
                                        s32 selectedOffset = byteOffset + 8;
                                        {
                                            register u32 *selected asm("$2") =
                                                (u32 *)((u8 *)colours + selectedOffset);
                                            gte_lwc2_6_0(selected);
                                        }
                                    } else
                                        goto no_override;
                                }
                            shade_override:
                                gte_cop2_hazard_slot();
                                gte_cop2_hazard_slot();
                                gte_ncct_command();
                                Draw_StoreColours(altColours);
                            no_override:;
                                vertexIndex += 3;
                                mainColours += 3;
                                altColours += 3;
                            } while (vertexIndex < part->vertex_count);
                        }
                    }
                }
                matrix += 8;
                __asm__("" : "=r"(entity) : "0"(entity), "r"(matrix));
                count = entity->header->part_count;
                __asm__("" : "=r"(partIndex) : "0"(partIndex), "r"(count));
                partIndex++;
                __asm__("" : "=r"(offset) : "0"(offset), "r"(partIndex));
                offset += sizeof(RenderObjectPart);
            } while (partIndex < count);
        }
    }
}

#define NULL ((void *)0)
extern u32 g_RenderClutLookupTable[];

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
        tpageTbl = D_800A6360;
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
