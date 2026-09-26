/* Matching debt: register pins, empty barriers, volatile shade reads,
 * gotos, and an artificial 16-byte local frame reservation. */
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
extern RenderVec3s D_80091A58[];
extern u32 D_8009CDA0;

#define Draw_LoadMatrix(m, n)                                                                      \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        x = m[0];                                                                                  \
        y = m[1];                                                                                  \
        gte_ctc2_##n(x);                                                                           \
        LIGHT_NEXT(y);                                                                             \
        x = m[2];                                                                                  \
        y = m[3];                                                                                  \
        z = m[4];                                                                                  \
        LIGHT_2(x);                                                                                \
        LIGHT_3(y);                                                                                \
        LIGHT_4(z);                                                                                \
    }
#define LIGHT_NEXT gte_ctc2_1
#define LIGHT_2 gte_ctc2_2
#define LIGHT_3 gte_ctc2_3
#define LIGHT_4 gte_ctc2_4
#define Draw_LoadRotation(m) Draw_LoadMatrix(m, 0)
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
#define Draw_LoadAxis(src)                                                                         \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        x = src[0];                                                                                \
        y = src[3];                                                                                \
        z = src[6];                                                                                \
        gte_mtc2_9(x);                                                                             \
        gte_mtc2_10(y);                                                                            \
        gte_mtc2_11(z);                                                                            \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_mvmva_rotation_ir_sf12();                                                              \
    }
#define Draw_StoreAxis(dst)                                                                        \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
        dst[0] = x;                                                                                \
        dst[3] = y;                                                                                \
        dst[6] = z;                                                                                \
    }
#define Draw_StoreColours(out)                                                                     \
    {                                                                                              \
        register u32 *out1 asm("$3") = out + 1;                                                    \
        register u32 *out2 asm("$2") = out + 2;                                                    \
        gte_swc2_20_0(out);                                                                         \
        gte_swc2_21_0(out1);                                                                        \
        gte_swc2_22_0(out2);                                                                        \
    }
void Render_DrawObject(RenderObjectEntity *input, union RenderLightingMatrix *view) {
    register RenderObjectEntity *entity asm("$11") = input;
    register s32 *viewMatrix asm("$19") = (s32 *)view;
    register volatile u32 *baseColour asm("$18") = (volatile u32 *)0x1F800000;
    register volatile s16 *lightMatrix asm("$5") = (s16 *)0x1F800004;
    register RenderVec3s *normals asm("$16");
    register s32 offset asm("$17");
    register s32 *matrix asm("$24");
    register s32 partIndex asm("$25");
    register RenderObjectPart *part asm("$15");
    register s32 vertexIndex asm("$10");
    register s32 first asm("$12");
    register u32 *mainColours asm("$9");
    register u32 *altColours asm("$8");
    register s16 *normalIndex asm("$7");
    u32 frameReserve[4];
    register u8 shadeR asm("$20"), shadeG asm("$21"), shadeB asm("$22");
    __asm__("" : "=r"(entity) : "0"(entity));
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
        register s32 count asm("$2") = entity->header->part_count;
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
                    __asm__("" : "=r"(lightMatrix) : "0"(lightMatrix));
                    Draw_LoadRotation(viewMatrix);
                    Draw_LoadAxis(((u16 *)matrix));
                    Draw_StoreAxis(lightMatrix);
                    __asm__("" : "=r"(matrix) : "0"(matrix) : "memory");
                    {
                        register u16 *src asm("$2") = (u16 *)matrix + 1;
                        register volatile s16 *dst asm("$2");
                        __asm__("" : "=r"(src) : "0"(src));
                        Draw_LoadAxis(src);
                        dst = lightMatrix + 1;
                        __asm__("" : "=r"(dst) : "0"(dst));
                        Draw_StoreAxis(dst);
                    }
                    {
                        register u16 *src asm("$2") = (u16 *)matrix + 2;
                        register volatile s16 *dst asm("$2");
                        __asm__("" : "=r"(src) : "0"(src));
                        Draw_LoadAxis(src);
                        dst = lightMatrix + 2;
                        __asm__("" : "=r"(dst) : "0"(dst));
                        Draw_StoreAxis(dst);
                    }
                    Draw_LoadLight(((volatile s32 *)lightMatrix));
                    vertexIndex = 0;
                    first = part->vertex_start;
                    {
                        register s32 byteOffset asm("$3") = first * 4;
                        register u32 *colourBase asm("$2") = D_800B1638;
                        mainColours = (u32 *)((u8 *)colourBase + byteOffset);
                        colourBase = D_800A6360;
                        altColours = (u32 *)((u8 *)colourBase + byteOffset);
                    }
                    {
                        register s32 vertexOffset asm("$4") = first * 8;
                        register RenderVec3s *vertices asm("$2") = entity->vertices;
                        register s32 vertexCount asm("$3");
                        __asm__("" : "=r"(part) : "0"(part), "r"(vertices));
                        vertexCount = part->vertex_count;
                        __asm__("" : "=r"(vertexCount) : "0"(vertexCount));
                        if (vertexCount > 0) {
                            vertices = (RenderVec3s *)((u8 *)vertices + vertexOffset);
                            normalIndex = &vertices[2].pad;
                            do {
                                {
                                    register s32 a asm("$4") = normalIndex[-8];
                                    register s32 b asm("$3") = normalIndex[-4];
                                    register s32 c asm("$2") = normalIndex[0];
                                    register RenderVec3s *na asm("$4"), *nb asm("$3"),
                                        *nc asm("$2");
                                    na = (RenderVec3s *)((u32)(a * 8) + (u32)normals);
                                    nb = (RenderVec3s *)((u32)(b * 8) + (u32)normals);
                                    nc = (RenderVec3s *)((u32)(c * 8) + (u32)normals);
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
                                    register u32 *colours asm("$6") = entity->vertex_colours;
                                    register s32 byteOffset asm("$4");
                                    register u8 *colour asm("$3");
                                    __asm__("" : "=r"(index) : "0"(index));
                                    byteOffset = index * 4;
                                    colour = (u8 *)((u32)byteOffset + (u32)colours);
                                    __asm__("" : "=r"(colours) : "0"(colours));
                                    if (colour[3]) {
                                        gte_lwc2_6_0(colour);
                                        goto shade_override;
                                    } else if (colour[7]) {
                                        register s32 selectedOffset asm("$2") = byteOffset + 4;
                                        __asm__("" : "=r"(selectedOffset) : "0"(selectedOffset));
                                        {
                                            register u32 *selected asm("$2") =
                                                (u32 *)((u8 *)colours + selectedOffset);
                                            gte_lwc2_6_0(selected);
                                        }
                                        goto shade_override;
                                    } else if (colour[11]) {
                                        register s32 selectedOffset asm("$2") = byteOffset + 8;
                                        __asm__("" : "=r"(selectedOffset) : "0"(selectedOffset));
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
        __asm__("" : "=m"(frameReserve));
    }
}
