/* MASPSX_FLAGS: --dont-expand-li */
#include "fx_common_render.h"
#include "pe1/gte.h"

/* Reconstructs the eight polygon streams and the FT3/FT4 subdivision paths.
 * Retail range 0x80197BA0..0x8019959C; source boundary is provisional. */
void FxCommon_DrawPolyResource(void *context, FxCommonPolyResource *resource) {
    register FxCommonRenderScratchpad *scratchpad asm("$16");
    FxCommonVector *quadMidpoint13;
    FxCommonBuffer *buffer;
    FxCommonBuffer *orderBuffer;
    register s32 resource_offset0 asm("$2");
    s32 resource_offset2;
    FxGt3CursorSlot gt3Slot;
    FxGt4CursorSlot gt4Slot;
    FxCommonVector *quadMidpoint23;
    FxCommonTexturedQuad *var_fp;
    register u8 *var_s2 asm("$18");
    u8 *var_s3;
    u8 *var_s5;
    FxCommonColoredQuad *var_s7;
    u8 *var_t0;
    FxCommonTexturedTriangle *var_t3;
    u8 *var_t5;
    u8 *var_t6;
    u32 trianglePacketAddress24;
    s32 temp_a0;
    u32 temp_v0;
    u8 *orderingTableBase;
    register s32 var_s4 asm("$20");
    register u8 *var_s1 asm("$17");
    FxCommonFt4Packet *var_t1_2;
    FxCommonPolyResource *var_t8;
    s32 primitiveCount;
    s32 initialDepthBias;

    register s32 resourceOffset7 asm("$2");
    var_t8 = resource;
    var_s4 = 0;
    resource_offset0 = var_t8->offsets[0];
    resource_offset2 = var_t8->offsets[2];
    buffer = D_8019C9C0;
    var_s3 = ((u8 *)var_t8) + resource_offset0;
    var_t0 = ((u8 *)var_t8) + resource_offset2;
    resource_offset0 = var_t8->offsets[1];
    resource_offset2 = var_t8->offsets[4];
    var_s5 = ((u8 *)var_t8) + resource_offset0;
    var_t3 = (FxCommonTexturedTriangle *)(((u8 *)var_t8) + resource_offset2);
    resource_offset0 = var_t8->offsets[3];
    resource_offset2 = var_t8->offsets[6];
    var_s7 = (FxCommonColoredQuad *)(((u8 *)var_t8) + resource_offset0);
    resource_offset0 = var_t8->offsets[5];
    gt3Slot.cursor = (FxCommonColoredTexturedTriangle *)(((u8 *)var_t8) + resource_offset2);
    initialDepthBias = D_801EA5E0;
    asm volatile("" : "=r"(resource_offset0) : "0"(resource_offset0), "r"(initialDepthBias));
    var_fp = (FxCommonTexturedQuad *)(((u8 *)var_t8) + resource_offset0);
    resourceOffset7 = var_t8->offsets[7];
    ((volatile FxCommonRenderScratchpad *)0x1F800000)->depthBias = initialDepthBias;
    asm volatile("" : : "r"(resourceOffset7), "r"(initialDepthBias));
    gt4Slot.cursor = (FxCommonColoredTexturedQuad *)(((u8 *)var_t8) + resourceOffset7);
    (void)&gt3Slot.cursor;
    (void)&gt4Slot.cursor;
    {
        u8 stackPadding[60];
        var_s2 = buffer->data;
        scratchpad = (FxCommonRenderScratchpad *)0x1F800000;
        primitiveCount = var_t8->counts[0];
        if (primitiveCount > 0) {
            u32 temp_a0_2;
            s32 temp_v1;
            u32 var_t1;
            u32 addressMask0;
            u32 lengthMask0;
            addressMask0 = 0xFFFFFF;
            lengthMask0 = 0xFF000000;
            var_t1 = lengthMask0;
            var_s1 = var_s2 + 4;
            do {
                for (;;) {
                    temp_v1 = func_80079384(
                        &((FxCommonFlatTriangle *)var_s3)->vertices[0],
                        &((FxCommonFlatTriangle *)var_s3)->vertices[1],
                        &((FxCommonFlatTriangle *)var_s3)->vertices[2],
                        &scratchpad->screenCoordinates[0], &scratchpad->screenCoordinates[1],
                        &scratchpad->screenCoordinates[2], &scratchpad->perspective,
                        &scratchpad->orderingDepth, &scratchpad->transformFlags);
                    temp_a0 = scratchpad->orderingDepth;
                    if (temp_a0 > 0) {
                        scratchpad->orderingDepth = (temp_a0 + D_801EA5E0) >> 2;
                        if (temp_v1 > 0) {
                            ((FxCommonF3Packet *)(var_s1 - 0x4))->tag.bytes.length = 4;
                            ((FxCommonF3Packet *)(var_s1 - 0x4))->xy0 =
                                scratchpad->screenCoordinates[0];
                            ((FxCommonF3Packet *)(var_s1 - 0x4))->xy1 =
                                scratchpad->screenCoordinates[1];
                            ((FxCommonF3Packet *)(var_s1 - 0x4))->xy2 =
                                scratchpad->screenCoordinates[2];
                            *((u32 *)&((FxCommonF3Packet *)(var_s1 - 0x4))->color) =
                                ((FxCommonFlatTriangle *)var_s3)->color;
                            var_s1 += 0x14;
                            temp_a0_2 = ((FxCommonPacketTag *)var_s2)->packed;
                            orderBuffer = D_8019C9C0;
                            temp_a0_2 = (((FxCommonPacketTag *)var_s2)->packed) & var_t1;
                            temp_v0 =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                      addressMask0);
                            temp_a0_2 |= temp_v0;
                            ((FxCommonF3Packet *)var_s2)->tag.packed = temp_a0_2;
                            temp_a0_2 = ((u32)var_s2) & addressMask0;
                            var_s2 += 0x14;
                            orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                 var_t1) |
                                temp_a0_2;
                        }
                    }
                    break;
                }
                var_s4 += 1;
                var_s3 += 0x1C;
            } while (var_s4 < var_t8->counts[0]);
        }
        {
            s32 primitiveIndex1;
            primitiveIndex1 = 0;
            primitiveCount = var_t8->counts[1];
            if (primitiveCount > 0) {
                s32 temp_a0_3;
                u32 temp_a0_4;
                register s32 temp_v1_3 asm("$3");
                u8 *new_var3;
                u32 addressMask1;
                u32 lengthMask1;
                addressMask1 = 0xFFFFFF;
                lengthMask1 = 0xFF000000;
                var_s1 = var_s2 + 4;
                do {
                    for (;;) {
                        temp_v1_3 = func_80079414(
                            &((FxCommonFlatQuad *)var_s5)->vertices[0],
                            &((FxCommonFlatQuad *)var_s5)->vertices[1],
                            &((FxCommonFlatQuad *)var_s5)->vertices[2],
                            &((FxCommonFlatQuad *)var_s5)->vertices[3],
                            &scratchpad->screenCoordinates[0], &scratchpad->screenCoordinates[1],
                            &scratchpad->screenCoordinates[2], &scratchpad->screenCoordinates[3],
                            &scratchpad->perspective, &scratchpad->orderingDepth,
                            &scratchpad->transformFlags);
                        temp_a0_3 = scratchpad->orderingDepth;
                        if (temp_a0_3 > 0) {
                            scratchpad->orderingDepth = (temp_a0_3 + D_801EA5E0) >> 2;
                            if (temp_v1_3 > 0) {
                                ((FxCommonF4Packet *)(var_s1 - 0x4))->tag.bytes.length = 5;
                                ((FxCommonF4Packet *)(var_s1 - 0x4))->xy0 =
                                    scratchpad->screenCoordinates[0];
                                ((FxCommonF4Packet *)(var_s1 - 0x4))->xy1 =
                                    scratchpad->screenCoordinates[1];
                                ((FxCommonF4Packet *)(var_s1 - 0x4))->xy2 =
                                    scratchpad->screenCoordinates[2];
                                ((FxCommonF4Packet *)(var_s1 - 0x4))->xy3 =
                                    scratchpad->screenCoordinates[3];
                                *((u32 *)&((FxCommonF4Packet *)(var_s1 - 0x4))->color) =
                                    ((FxCommonFlatQuad *)var_s5)->color;
                                var_s1 += 0x18;
                                temp_a0_4 = ((FxCommonPacketTag *)var_s2)->packed;
                                orderBuffer = D_8019C9C0;
                                temp_a0_4 = (((FxCommonPacketTag *)var_s2)->packed) & lengthMask1;
                                temp_v0 = ((orderBuffer->allocation[scratchpad->orderingDepth]
                                                     .packed) &
                                                addressMask1);
                                new_var3 = var_s2;
                                temp_a0_4 |= temp_v0;
                                ((FxCommonPacketTag *)new_var3)->packed = temp_a0_4;
                                temp_a0_4 = ((u32)var_s2) & addressMask1;
                                var_s2 += 0x18;
                                orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                    ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                     lengthMask1) |
                                    temp_a0_4;
                            }
                        }
                        break;
                    }
                    primitiveIndex1 += 1;
                    var_s5 += 0x24;
                } while (primitiveIndex1 < var_t8->counts[1]);
            }
        }
        {
            s32 primitiveIndex2;
            primitiveIndex2 = 0;
            primitiveCount = var_t8->counts[2];
            if (primitiveCount > 0) {
                s32 temp_a0_5;
                u32 temp_a0_6;
                register s32 temp_v1_5 asm("$3");
                u32 addressMask2;
                u32 lengthMask2;
                addressMask2 = 0xFFFFFF;
                lengthMask2 = 0xFF000000;
                var_s3 = var_t0 + 8;
                var_s1 = var_s2 + 0x14;
                do {
                    temp_v1_5 = func_80079384(
                        &((FxCommonColoredTriangle *)var_t0)->vertices[0],
                        &((FxCommonColoredTriangle *)var_t0)->vertices[1],
                        &((FxCommonColoredTriangle *)var_t0)->vertices[2],
                        &scratchpad->screenCoordinates[0], &scratchpad->screenCoordinates[1],
                        &scratchpad->screenCoordinates[2], &scratchpad->perspective,
                        &scratchpad->orderingDepth, &scratchpad->transformFlags);
                    temp_a0_5 = scratchpad->orderingDepth;
                    if (temp_a0_5 > 0) {
                        scratchpad->orderingDepth = (temp_a0_5 + D_801EA5E0) >> 2;
                        if (temp_v1_5 > 0) {
                            ((FxCommonG3Packet *)(var_s1 - 0x14))->tag.bytes.length = 6;
                            ((FxCommonG3Packet *)(var_s1 - 0x14))->xy0 =
                                scratchpad->screenCoordinates[0];
                            ((FxCommonG3Packet *)(var_s1 - 0x14))->xy1 =
                                scratchpad->screenCoordinates[1];
                            ((FxCommonG3Packet *)(var_s1 - 0x14))->xy2 =
                                scratchpad->screenCoordinates[2];
                            ((FxCommonG3Packet *)(var_s1 - 0x14))->color0 =
                                ((FxCommonColoredTriangle *)var_t0)->colors[0];
                            ((FxCommonG3Packet *)(var_s1 - 0x14))->color1 =
                                (*((u32 *)&((FxCommonColoredTriangle *)(var_s3 - 8))->colors[1]));
                            *((u32 *)&((FxCommonG3Packet *)(var_s1 - 0x14))->color2) =
                                (*((u32 *)&((FxCommonColoredTriangle *)(var_s3 - 8))->colors[2]));
                            var_s1 += 0x1C;
                            temp_a0_6 = ((FxCommonPacketTag *)var_s2)->packed;
                            orderBuffer = D_8019C9C0;
                            temp_a0_6 = (((FxCommonPacketTag *)var_s2)->packed) & lengthMask2;
                            temp_v0 =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                      addressMask2);
                            temp_a0_6 |= temp_v0;
                            ((FxCommonPacketTag *)var_s2)->packed = temp_a0_6;
                            temp_a0_6 = ((u32)var_s2) & addressMask2;
                            var_s2 += 0x1C;
                            orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                 lengthMask2) |
                                temp_a0_6;
                        }
                    }
                    primitiveIndex2 += 1;
                    var_s3 += 0x24;
                    var_t0 += 0x24;
                } while (primitiveIndex2 < var_t8->counts[2]);
            }
        }
        {
            s32 primitiveIndex3;
            primitiveIndex3 = 0;
            primitiveCount = var_t8->counts[3];
            if (primitiveCount > 0) {
                s32 temp_a0_7;
                u32 temp_a0_8;
                register s32 temp_v1_7 asm("$3");
                u32 addressMask3;
                u32 lengthMask3;
                addressMask3 = 0xFFFFFF;
                lengthMask3 = 0xFF000000;
                var_s3 = (u8 *)&var_s7->colors[3];
                var_s1 = var_s2 + 0x1C;
                do {
                    temp_v1_7 = func_80079414(
                        &var_s7->vertices[0], &var_s7->vertices[1], &var_s7->vertices[2],
                        &var_s7->vertices[3], &scratchpad->screenCoordinates[0],
                        &scratchpad->screenCoordinates[1], &scratchpad->screenCoordinates[2],
                        &scratchpad->screenCoordinates[3], &scratchpad->perspective,
                        &scratchpad->orderingDepth, &scratchpad->transformFlags);
                    temp_a0_7 = scratchpad->orderingDepth;
                    if (temp_a0_7 > 0) {
                        scratchpad->orderingDepth = (temp_a0_7 + D_801EA5E0) >> 2;
                        if (temp_v1_7 > 0) {
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->tag.bytes.length = 8;
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->xy0 =
                                scratchpad->screenCoordinates[0];
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->xy1 =
                                scratchpad->screenCoordinates[1];
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->xy2 =
                                scratchpad->screenCoordinates[2];
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->xy3 =
                                scratchpad->screenCoordinates[3];
                            asm volatile("" : "=r"(var_s3) : "0"(var_s3));
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->color0 = var_s7->colors[0];
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->color1 =
                                ((FxCommonColoredQuad *)(var_s3 - 12))->colors[1];
                            ((FxCommonG4Packet *)(var_s1 - 0x1c))->color2 =
                                (*((u32 *)&((FxCommonColoredQuad *)(var_s3 - 12))->colors[2]));
                            *((u32 *)&((FxCommonG4Packet *)(var_s1 - 0x1c))->color3) =
                                ((FxCommonColoredQuad *)(var_s3 - 12))->colors[3];
                            var_s1 += 0x24;
                            temp_a0_8 = ((FxCommonPacketTag *)var_s2)->packed;
                            orderBuffer = D_8019C9C0;
                            temp_a0_8 = (((FxCommonPacketTag *)var_s2)->packed) & lengthMask3;
                            temp_v0 =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                      addressMask3);
                            temp_a0_8 |= temp_v0;
                            ((FxCommonPacketTag *)var_s2)->packed = temp_a0_8;
                            temp_a0_8 = ((u32)var_s2) & addressMask3;
                            var_s2 += 0x24;
                            orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                 lengthMask3) |
                                temp_a0_8;
                        }
                    }
                    primitiveIndex3 += 1;
                    var_s3 += 0x30;
                    ++var_s7;
                } while (primitiveIndex3 < var_t8->counts[3]);
            }
            var_t0 = var_s2;
        }
        {
            FxCommonFt3Packet *trianglePacketCursor;
            FxCommonFt3Packet *trianglePacket1Address;
            u32 *triangleScreen2Address;
            u32 *triangleScreen1Address;
            u32 *triangleMidpointScreen4Address;
            register s32 primitiveIndex4 asm("$20");
            trianglePacketCursor = (FxCommonFt3Packet *)var_t0;
            primitiveIndex4 = 0;
            primitiveCount = var_t8->counts[4];
            if (primitiveCount > 0) {
                register u32 *triangleGteScreen0 asm("$4");
                register u32 *triangleGteScreen1 asm("$3");
                s32 *triangleGteScreen2;
                FxCommonVector *triangleMidpoint02;
                FxCommonVector *triangleMidpoint12;
                register u8 *var_a3 asm("$7");
                register FxCommonVector *var_t4 asm("$12");
                u32 temp_a0_13;
                s32 temp_a0_9;
                s32 temp_v1_9;
                u16 *triangleUv2Cursor;
                u16 var_v0;
                u32 triangleLength = 7;
                u32 triangleAddressMask = 0xFFFFFF;
                register u32 triangleLengthMask asm("$10") = 0xFF000000;
                var_a3 = (u8 *)&var_t3->textures[1].uv2;
                var_t6 = (u8 *)&var_t3->vertices[2];
                var_t5 = (u8 *)&var_t3->vertices[1];
                var_t4 = &var_t3->vertices[0];
                triangleUv2Cursor = &trianglePacketCursor->uv2;
                do {
                    gte_ldvxy0_precise(var_t4);
                    gte_ldvz0_precise(var_t4);
                    gte_ldvxy1_precise(var_t5);
                    gte_lwc2_3_4(var_t5);
                    gte_ldvxy2_precise(var_t6);
                    gte_ldvz2_precise(var_t6);
                    gte_rtpt_padded();
                    gte_nclip();
                    gte_stmac0_precise(&scratchpad->normalClip);
                    if (!(scratchpad->normalClip > 0)) {
                        goto nextTexturedTriangle;
                    }
                    gte_avsz3_padded();
                    gte_stotz_precise(&scratchpad->orderingDepth);
                    temp_a0_9 = (scratchpad->orderingDepth + scratchpad->depthBias) >> 2;
                    scratchpad->orderingDepth = temp_a0_9;
                    if (((temp_a0_9 * 4) - scratchpad->depthBias) <= 0) {
                        goto nextTexturedTriangle;
                    }
                    if (temp_a0_9 < 0x1F4) {
                        register FxCommonVector *triangleMidpoint01 asm("$4");
                        FxCommonVector *triangleNamedInput2;
                        FxCommonTextureSet *triangleTexture;
                        u32 *triangleMidpointOutput0;
                        u32 *triangleMidpointOutput2;
                        scratchpad->vertices[1] = (FxCommonVector *)var_t5;
                        scratchpad->vertices[2] = var_t4;
                        scratchpad->vertices[3] = (FxCommonVector *)var_t6;
                        triangleMidpoint12 = &scratchpad->midpoints[3];
                        scratchpad->midpoints[2].x =
                            (((scratchpad->vertices[1]->x)) + (scratchpad->vertices[2]->x)) >> 1;
                        scratchpad->midpoints[2].y =
                            (((scratchpad->vertices[1]->y)) + (scratchpad->vertices[2]->y)) >> 1;
                        scratchpad->midpoints[2].z =
                            (((scratchpad->vertices[1]->z)) + (scratchpad->vertices[2]->z)) >> 1;
                        triangleMidpoint02 = &scratchpad->midpoints[4];
                        triangleMidpoint02->x =
                            ((scratchpad->vertices[3]->x) + (scratchpad->vertices[2]->x)) >> 1;
                        scratchpad->midpoints[4].y =
                            ((scratchpad->vertices[3]->y) + (scratchpad->vertices[2]->y)) >> 1;
                        scratchpad->midpoints[4].z =
                            ((scratchpad->vertices[3]->z) + (scratchpad->vertices[2]->z)) >> 1;
                        triangleMidpoint12->x =
                            (((scratchpad->vertices[1]->x)) + (scratchpad->vertices[3]->x)) >> 1;
                        scratchpad->midpoints[3].y =
                            (((scratchpad->vertices[1]->y)) + (scratchpad->vertices[3]->y)) >> 1;
                        scratchpad->midpoints[3].z =
                            (((scratchpad->vertices[1]->z)) + (scratchpad->vertices[3]->z)) >> 1;
                        triangleMidpoint01 = &scratchpad->midpoints[2];
                        triangleNamedInput2 = &scratchpad->midpoints[3];
                        asm volatile("" : : "r"(triangleNamedInput2));
                        gte_ldvxy0_precise(triangleMidpoint01);
                        gte_ldvz0_precise(triangleMidpoint01);
                        gte_ldvxy1_precise(triangleMidpoint02);
                        gte_ldvz1_precise(triangleMidpoint02);
                        gte_ldvxy2_precise(triangleNamedInput2);
                        gte_ldvz2_precise(triangleNamedInput2);
                        gte_rtpt_padded();
                        triangleTexture = &((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0];
                        scratchpad->midpointUv[4].components.u =
                            ((triangleTexture->uv0.components.u) +
                             (triangleTexture->uv2.components.u)) >>
                            1;
                        scratchpad->midpointUv[4].components.v =
                            ((triangleTexture->uv0.components.v) +
                             (triangleTexture->uv2.components.v)) >>
                            1;
                        scratchpad->midpointUv[2].components.u =
                            ((triangleTexture->uv0.components.u) +
                             (triangleTexture->uv1.components.u)) >>
                            1;
                        triangleMidpointScreen4Address = &scratchpad->midpointScreenCoordinates[4];
                        triangleMidpointOutput0 = &scratchpad->midpointScreenCoordinates[2];
                        asm volatile(""
                                     : "=r"(triangleMidpointOutput0)
                                     : "0"(triangleMidpointOutput0));
                        triangleMidpointOutput2 = &scratchpad->midpointScreenCoordinates[3];
                        asm volatile(""
                                     : "=r"(triangleMidpointOutput2)
                                     : "0"(triangleMidpointOutput2));
                        gte_stsxy0_precise(triangleMidpointOutput0);
                        gte_stsxy1_precise(&scratchpad->midpointScreenCoordinates[4]);
                        gte_stsxy2_mem(triangleMidpointOutput2);
                        gte_ldvxy0_precise(var_t4);
                        gte_ldvz0_precise(var_t4);
                        gte_lwc2_2_0(var_t5);
                        gte_lwc2_3_4(var_t5);
                        gte_lwc2_4_0(var_t6);
                        gte_lwc2_5_4(var_t6);
                        gte_rtpt_padded();
                        scratchpad->midpointUv[2].components.v =
                            ((triangleTexture->uv0.components.v) +
                             (triangleTexture->uv1.components.v)) >>
                            1;
                        scratchpad->midpointUv[3].components.u =
                            (u8)((triangleTexture->uv1.components.u +
                                       (triangleTexture->uv2.components.u)) >>
                                 1);
                        scratchpad->midpointUv[3].components.v =
                            (u8)(((triangleTexture->uv1.components.v) +
                                  (triangleTexture->uv2.components.v)) >>
                                 1);
                        triangleScreen1Address = &scratchpad->screenCoordinates[1];
                        triangleScreen2Address = (u32 *)(&scratchpad->screenCoordinates[2]);
                        asm volatile("" : : "r"(triangleScreen2Address));
                        gte_stsxy0_precise(&scratchpad->screenCoordinates[0]);
                        gte_stsxy1_precise(&scratchpad->screenCoordinates[1]);
                        gte_stsxy2_precise(&scratchpad->screenCoordinates[2]);
                        trianglePacket1Address = trianglePacketCursor + 1;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->tag.bytes.length =
                            triangleLength;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->color = var_t3->color;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy0 =
                            scratchpad->screenCoordinates[0];
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy1 =
                            scratchpad->midpointScreenCoordinates[2];
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy2 =
                            scratchpad->midpointScreenCoordinates[4];
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv0);
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv1);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0 =
                            (u16)(((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[0]
                                      .uv0.packed);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1 =
                            (u16)scratchpad->midpointUv[2].packed;
                        *triangleUv2Cursor =
                            (u16)scratchpad->midpointUv[4].packed;
                        {
                            u32 firstFt3Tag = trianglePacketCursor->tag.packed;
                            triangleUv2Cursor += 16;
                            orderBuffer = D_8019C9C0;
                            firstFt3Tag &= triangleLengthMask;
                            firstFt3Tag |=
                                orderBuffer->allocation[scratchpad->orderingDepth].packed &
                                triangleAddressMask;
                            trianglePacketCursor->tag.packed = firstFt3Tag;
                        }

                        orderBuffer->allocation[scratchpad->orderingDepth].packed =
                            (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                             triangleLengthMask) |
                            (((u32)trianglePacketCursor) & triangleAddressMask);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->tag.bytes.length =
                            triangleLength;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->color = var_t3->color;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy0 =
                            scratchpad->midpointScreenCoordinates[2];
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy1 =
                            *triangleScreen1Address;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy2 =
                            scratchpad->midpointScreenCoordinates[3];
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv0);
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv1);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0 =
                            (u16)scratchpad->midpointUv[2].packed;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1 =
                            (u16)(((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[0]
                                      .uv1.packed);
                        ++trianglePacketCursor;
                        *((u16 *)(triangleUv2Cursor)) = (u16)scratchpad->midpointUv[3].packed;
                        asm volatile("" : "=r"(triangleUv2Cursor) : "0"(triangleUv2Cursor));
                        triangleUv2Cursor += 16;
                        temp_a0 = trianglePacketCursor->tag.packed;
                        orderBuffer = D_8019C9C0;
                        temp_a0 &= triangleLengthMask;
                        temp_a0 |= ((D_8019C9C0->allocation[scratchpad->orderingDepth].packed) &
                                    triangleAddressMask);
                        trianglePacketCursor->tag.packed = temp_a0;
                        trianglePacketAddress24 = (s32)trianglePacketCursor;
                        trianglePacketAddress24 = triangleAddressMask & trianglePacketAddress24;
                        orderBuffer->allocation[scratchpad->orderingDepth].packed =
                            (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                             triangleLengthMask) |
                            ((trianglePacketAddress24)&triangleAddressMask);
                        ++trianglePacketCursor;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->tag.bytes.length =
                            triangleLength;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->color = var_t3->color;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy0 =
                            scratchpad->midpointScreenCoordinates[2];
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy1 =
                            scratchpad->midpointScreenCoordinates[3];
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy2 =
                            scratchpad->midpointScreenCoordinates[4];
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv0);
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv1);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0 =
                            (u16)scratchpad->midpointUv[2].packed;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1 =
                            (u16)(scratchpad->midpointUv[3].packed);
                        *triangleUv2Cursor = (u16)scratchpad->midpointUv[4].packed;
                        triangleUv2Cursor += 16;
                        {
                            u32 thirdTriangleTag = trianglePacketCursor->tag.packed;
                            orderBuffer = D_8019C9C0;
                            thirdTriangleTag &= triangleLengthMask;
                            thirdTriangleTag |=
                                (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                                 triangleAddressMask);
                            trianglePacketCursor->tag.packed = thirdTriangleTag;
                        }
                        orderBuffer->allocation[scratchpad->orderingDepth].packed =
                            (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                             triangleLengthMask) |
                            (((u32)trianglePacketCursor) & triangleAddressMask);
                        ++trianglePacketCursor;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->tag.bytes.length =
                            triangleLength;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->color = var_t3->color;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy0 =
                            scratchpad->midpointScreenCoordinates[3];
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy1 =
                            *triangleScreen2Address;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->xy2 =
                            *triangleMidpointScreen4Address;
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv0);
                        *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                            *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv1);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0 =
                            (u16)(scratchpad->midpointUv[3].packed);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1 =
                            (u16)(((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[0]
                                      .uv2.packed);
                        var_v0 = (u16)scratchpad->midpointUv[4].packed;
                    } else {

                        triangleGteScreen0 = &trianglePacketCursor->xy0;
                        triangleGteScreen2 = &trianglePacketCursor->xy2;
                        triangleGteScreen1 = &trianglePacketCursor->xy1;
                        gte_stsxy0_precise(triangleGteScreen0);
                        gte_stsxy1_precise(triangleGteScreen1);
                        gte_stsxy2_precise(triangleGteScreen2);
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->tag.bytes.length =
                            triangleLength;
                        ((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->color = var_t3->color;
                        temp_v1_9 = scratchpad->orderingDepth;
                        if (temp_v1_9 < 0x1FE) {
                            *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                                *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[0]
                                      .uv0);
                            *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                                *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[0]
                                      .uv1);
                            var_v0 =
                                ((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[0].uv2.packed;
                        } else if (temp_v1_9 >= 0x2BD) {
                            *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                                *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[2]
                                      .uv0);
                            *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                                *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[2]
                                      .uv1);
                            var_v0 =
                                ((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[2].uv2.packed;
                        } else {
                            *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv0) =
                                *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[1]
                                      .uv0);
                            *((s32 *)&((FxCommonFt3Packet *)(triangleUv2Cursor - 14))->uv1) =
                                *((s32 *)&((FxCommonTexturedTriangle *)(var_a3 - 24))
                                      ->textures[1]
                                      .uv1);
                            var_v0 =
                                ((FxCommonTexturedTriangle *)(var_a3 - 24))->textures[1].uv2.packed;
                        }
                    }
                    *triangleUv2Cursor = var_v0;
                    triangleUv2Cursor += 16;
                    {
                        u32 finalFt3Tag = trianglePacketCursor->tag.packed;
                        orderBuffer = D_8019C9C0;
                        finalFt3Tag &= triangleLengthMask;
                        temp_v0 =
                            ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                  triangleAddressMask);
                        finalFt3Tag |= temp_v0;
                        trianglePacketCursor->tag.packed = finalFt3Tag;
                    }

                    temp_a0_13 = ((u32)trianglePacketCursor) & triangleAddressMask;
                    ++trianglePacketCursor;
                    orderBuffer->allocation[scratchpad->orderingDepth].packed =
                        (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                         triangleLengthMask) |
                        temp_a0_13;
                nextTexturedTriangle:
                    primitiveIndex4 += 1;

                    var_a3 += 0x40;
                    var_t6 += 0x40;
                    var_t5 += 0x40;
                    var_t4 += 8;
                    ++var_t3;
                } while (primitiveIndex4 < var_t8->counts[4]);
            }
            var_t1_2 = (FxCommonFt4Packet *)trianglePacketCursor;
        }
        var_s4 = 0;
        primitiveCount = var_t8->counts[5];
        if (primitiveCount > 0) {

            register u32 quadLengthMask asm("$17");
            register u8 *quadTextureCursor asm("$10");
            u16 *quadUv3Cursor;
            FxCommonVector *quadVertex3;
            s32 temp_a0_14;
            u32 quadLength;
            u32 quadAddressMask;
            ;
            quadLength = 9;
            quadAddressMask = 0xFFFFFF;
            quadLengthMask = 0xFF000000;
            asm volatile(
                ""
                : "=r"(scratchpad), "=r"(quadLength), "=r"(quadAddressMask), "=r"(quadLengthMask)
                : "0"(scratchpad), "1"(quadLength), "2"(quadAddressMask), "3"(quadLengthMask));
            quadTextureCursor = (u8 *)&var_fp->textures[0].clut;
            quadVertex3 = &var_fp->vertices[3];
            var_s5 = (u8 *)&var_fp->vertices[2];
            var_s3 = (u8 *)&var_fp->vertices[1];
            var_s2 = (u8 *)&var_fp->vertices[0];
            quadUv3Cursor = &var_t1_2->uv3;
            do {
                gte_ldvxy0_precise_keep(var_s2, quadVertex3);
                gte_ldvz0_precise(var_s2);
                gte_ldvxy1_precise(var_s3);
                gte_ldvz1_precise(var_s3);
                gte_ldvxy2_precise(var_s5);
                gte_ldvz2_precise(var_s5);
                gte_rtpt_padded();
                gte_nclip();
                gte_stmac0_precise(&scratchpad->normalClip);
                if (!(scratchpad->normalClip > 0)) {
                    goto nextTexturedQuad;
                }
                gte_avsz3_padded();
                gte_stotz_precise(&scratchpad->orderingDepth);
                temp_a0_14 = (scratchpad->orderingDepth + scratchpad->depthBias) >> 2;
                scratchpad->orderingDepth = temp_a0_14;
                if (((temp_a0_14 * 4) - scratchpad->depthBias) <= 0) {
                    goto nextTexturedQuad;
                }
                if (temp_a0_14 >= 0x1F5) {
                    register u32 *quadGteScreen0 asm("$4");
                    register u32 *quadGteScreen1 asm("$3");
                    u32 *quadGteScreen2;
                    if (temp_a0_14 < 0x1FE) {
                        *((s32 *)&((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv0) =
                            *((u32 *)&((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                  ->textures[0]
                                  .uv0);
                        *((s32 *)&((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv1) =
                            *((u32 *)&((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                  ->textures[0]
                                  .uv1);
                        ((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv2 =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                ->textures[0]
                                .uv2.packed;
                        *quadUv3Cursor =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                ->textures[0]
                                .uv3.packed;
                    } else if (temp_a0_14 >= 0x2BD) {
                        *((s32 *)&((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv0) =
                            *((u32 *)&((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                  ->textures[2]
                                  .uv0);
                        *((s32 *)&((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv1) =
                            *((u32 *)&((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                  ->textures[2]
                                  .uv1);
                        ((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv2 =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                ->textures[2]
                                .uv2.packed;
                        *quadUv3Cursor =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                ->textures[2]
                                .uv3.packed;
                    } else {
                        *((s32 *)&((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv0) =
                            *((u32 *)&((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                  ->textures[1]
                                  .uv0);
                        *((s32 *)&((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv1) =
                            *((u32 *)&((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                  ->textures[1]
                                  .uv1);
                        ((FxCommonFt4Packet *)(quadUv3Cursor - 18))->uv2 =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                ->textures[1]
                                .uv2.packed;
                        *quadUv3Cursor =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                ->textures[1]
                                .uv3.packed;
                    }
                    quadGteScreen0 = &var_t1_2->xy0;
                    quadGteScreen1 = &var_t1_2->xy1;
                    quadGteScreen2 = &var_t1_2->xy2;
                    gte_stsxy0_precise(quadGteScreen0);
                    gte_stsxy1_precise(quadGteScreen1);
                    gte_stsxy2_precise(quadGteScreen2);
                    gte_lwc2_0_0(quadVertex3);
                    gte_lwc2_1_4(quadVertex3);
                    gte_rtps();
                    gte_stsxy2(&var_t1_2->xy3);
                    ((FxCommonFt4Packet *)(quadUv3Cursor - 18))->tag.bytes.length = quadLength;
                    ((FxCommonFt4Packet *)(quadUv3Cursor - 18))->color = var_fp->color;
                } else {
                    u32 *subdividedQuadGteScreen1;
                    u32 *quadMiddleOutput0;
                    u32 *quadMiddleOutput1;
                    u32 *subdividedQuadGteScreen3;
                    FxCommonUv *quadUv0;
                    register FxCommonUv *quadUv3;
                    u32 packetAddress24;
                    s16 midpoint13Z;
                    FxCommonUv *quadUv1;
                    FxCommonUv *quadUv2;
                    FxCommonVector *quadInput0;
                    FxCommonVector *quadInput1;
                    FxCommonVector *quadInput2;
                    FxCommonVector *quadCenterGteAddress;
                    u32 *quadMidpointGteScreen0;
                    u32 *quadLifetimeOutput0;
                    u32 *quadLifetimeOutput1;
                    FxCommonVector *quadCombinedIn0;
                    u32 *quadMidpointGteScreen1;
                    u32 *quadScreenValueAddress;
                    s32 midpoint23Z;
                    u32 *new_var;
                    s32 midpoint02X;
                    s32 midpoint02Y;
                    s32 quadEdgeY2;
                    s32 quadEdgeY3;
                    FxCommonVector *edgeX2;
                    FxCommonVector *edgeX3;
                    FxCommonVector *edgeY3;
                    s32 midpoint02Z;
                    u32 *midpointScreen2Address;
                    if (temp_a0_14 < 0x1FE) {
                        quadUv0 = &var_fp->textures[0].uv0;
                        var_t5 = (u8 *)&var_fp->textures[0].uv1;
                        var_t6 = (u8 *)&var_fp->textures[0].uv2;
                        quadUv3 = &var_fp->textures[0].uv3;
                    } else if (temp_a0_14 >= 0x2BD) {
                        quadUv0 = &var_fp->textures[2].uv0;
                        var_t5 = (u8 *)&var_fp->textures[2].uv1;
                        var_t6 = (u8 *)&var_fp->textures[2].uv2;
                        quadUv3 = &var_fp->textures[2].uv3;
                    } else {
                        quadUv0 = &var_fp->textures[1].uv0;
                        var_t5 = (u8 *)&var_fp->textures[1].uv1;
                        var_t6 = (u8 *)&var_fp->textures[1].uv2;
                        quadUv3 = &var_fp->textures[1].uv3;
                    }
                    quadUv1 = (FxCommonUv *)var_t5;
                    quadUv2 = (FxCommonUv *)var_t6;
                    scratchpad->vertices[1] = (FxCommonVector *)var_s3;
                    scratchpad->vertices[0] = (FxCommonVector *)var_s2;
                    scratchpad->vertices[2] = (FxCommonVector *)var_s5;
                    quadMidpoint13 = &scratchpad->midpoints[3];
                    scratchpad->vertices[3] = quadVertex3;
                    scratchpad->midpoints[0].x =
                        (((scratchpad->vertices[1]->x)) + (scratchpad->vertices[0]->x)) >> 1;
                    scratchpad->midpoints[0].y =
                        (((scratchpad->vertices[1]->y)) + (scratchpad->vertices[0]->y)) >> 1;
                    scratchpad->midpoints[0].z =
                        (((scratchpad->vertices[1]->z)) + (scratchpad->vertices[0]->z)) >> 1;
                    quadMidpoint23 = &scratchpad->midpoints[4];
                    scratchpad->midpoints[1].x =
                        ((scratchpad->vertices[0]->x) + (scratchpad->vertices[2]->x)) >> 1;
                    scratchpad->midpoints[1].y =
                        ((scratchpad->vertices[0]->y) + (scratchpad->vertices[2]->y)) >> 1;
                    scratchpad->midpoints[1].z =
                        ((scratchpad->vertices[0]->z) + (scratchpad->vertices[2]->z)) >> 1;
                    quadMidpoint13->x =
                        (((scratchpad->vertices[1]->x)) + (scratchpad->vertices[3]->x)) >> 1;
                    scratchpad->midpoints[3].y =
                        (((scratchpad->vertices[1]->y)) + (scratchpad->vertices[3]->y)) >> 1;
                    scratchpad->midpoints[3].z =
                        (((scratchpad->vertices[1]->z)) + (scratchpad->vertices[3]->z)) >> 1;
                    quadInput1 = &scratchpad->midpoints[1];
                    quadInput0 = &scratchpad->midpoints[0];
                    quadInput2 = &scratchpad->midpoints[3];
                    gte_ldvxy0_mem(quadInput0);
                    gte_ldvz0_mem(quadInput0);
                    gte_ldvxy1_mem(quadInput1);
                    gte_ldvz1_precise(quadInput1);
                    gte_ldvxy2_precise(quadInput2);
                    gte_ldvz2_precise(quadInput2);
                    gte_rtpt_padded();

                    edgeX2 = scratchpad->vertices[2];
                    edgeX3 = scratchpad->vertices[3];

                    edgeY3 = *(FxCommonVector *volatile *)&scratchpad->vertices[3];
                    midpoint02X = ((u16)((volatile FxCommonVector *)quadInput1)->x << 16);
                    quadMidpoint23->x = (edgeX2->x + edgeX3->x) >> 1;
                    asm volatile("" : : "r"(midpoint02X));
                    midpoint02X >>= 16;

                    quadEdgeY2 = scratchpad->vertices[2]->y;
                    quadEdgeY3 = edgeY3->y;

                    quadLifetimeOutput0 = &scratchpad->midpointScreenCoordinates[0];
                    midpoint02Y = ((volatile FxCommonVector *)quadInput1)->y;
                    quadMidpoint23->y = (quadEdgeY2 + quadEdgeY3) >> 1;
                    midpoint23Z =
                        ((scratchpad->vertices[2]->z) + (scratchpad->vertices[3]->z)) >> 1;
                    scratchpad->midpoints[2].x =
                        (midpoint02X + (((volatile FxCommonVector *)quadInput2)->x)) >> 1;
                    midpoint02Z = ((volatile FxCommonVector *)quadInput1)->z;
                    scratchpad->midpoints[2].y =
                        (midpoint02Y + (((volatile FxCommonVector *)quadInput2)->y)) >> 1;
                    midpoint13Z = (s16)(quadInput2->z);
                    quadMidpoint23->z = midpoint23Z;
                    scratchpad->midpoints[2].z = (midpoint02Z + midpoint13Z) >> 1;
                    do {
                    } while (0);
                    quadMidpointGteScreen1 = &scratchpad->midpointScreenCoordinates[1];
                    quadLifetimeOutput1 = &scratchpad->midpointScreenCoordinates[3];
                    asm volatile(""
                                 : "=r"(quadLifetimeOutput1)
                                 : "0"(quadLifetimeOutput1), "r"(quadMidpointGteScreen1));

                    gte_stsxy0_precise(quadLifetimeOutput0);
                    gte_stsxy1_precise(quadMidpointGteScreen1);
                    new_var = &scratchpad->midpointScreenCoordinates[3];
                    gte_stsxy2_mem(quadLifetimeOutput1);
                    quadCombinedIn0 = &scratchpad->midpoints[4];
                    asm volatile("" : : "r"(quadCombinedIn0));
                    quadCenterGteAddress = &scratchpad->midpoints[2];
                    asm volatile("" : "=r"(quadCenterGteAddress) : "0"(quadCenterGteAddress));
                    gte_ldvxy0_precise(quadCombinedIn0);
                    gte_ldvz0_precise(quadCombinedIn0);
                    gte_ldvxy1_precise(quadCenterGteAddress);
                    gte_ldvz1_precise(quadCenterGteAddress);
                    gte_ldvxy2_precise(var_s2);
                    gte_ldvz2_precise(var_s2);
                    gte_rtpt_padded();
                    scratchpad->midpointUv[0].components.u =
                        (quadUv0->components.u + quadUv1->components.u) >> 1;
                    scratchpad->midpointUv[0].components.v =
                        (quadUv0->components.v + quadUv1->components.v) >> 1;
                    scratchpad->midpointUv[1].components.u =
                        ((quadUv0->components.u + quadUv2->components.u) >> 1);
                    scratchpad->midpointUv[1].components.v =
                        ((quadUv0->components.v + quadUv2->components.v) >> 1);
                    scratchpad->midpointUv[3].components.u =
                        ((quadUv1->components.u + quadUv3->components.u) >> 1);
                    scratchpad->midpointUv[3].components.v =
                        ((quadUv1->components.v + quadUv3->components.v) >> 1);
                    quadMiddleOutput0 = &scratchpad->midpointScreenCoordinates[4];
                    quadMiddleOutput1 = &scratchpad->midpointScreenCoordinates[2];
                    asm volatile("" : : "r"(quadMiddleOutput1));
                    gte_stsxy0_precise(quadMiddleOutput0);
                    gte_stsxy1_precise(quadMiddleOutput1);
                    gte_stsxy2_precise(&scratchpad->screenCoordinates[0]);
                    gte_ldvxy0_precise(var_s3);
                    gte_ldvz0_precise(var_s3);
                    gte_ldvxy1_precise(var_s5);
                    gte_ldvz1_precise(var_s5);
                    gte_lwc2_4_0(quadVertex3);
                    gte_lwc2_5_4(quadVertex3);
                    gte_rtpt_padded();
                    subdividedQuadGteScreen3 = &scratchpad->screenCoordinates[3];
                    scratchpad->midpointUv[4].components.u =
                        (quadUv2->components.u + quadUv3->components.u) >> 1;
                    scratchpad->midpointUv[4].components.v =
                        (quadUv2->components.v + quadUv3->components.v) >> 1;
                    subdividedQuadGteScreen1 = &scratchpad->screenCoordinates[1];
                    scratchpad->midpointUv[2].components.u =
                        ((scratchpad->midpointUv[3].components.u) +
                         scratchpad->midpointUv[1].components.u) >>
                        1;
                    scratchpad->midpointUv[2].components.v =
                        (scratchpad->midpointUv[3].components.v +
                         scratchpad->midpointUv[1].components.v) >>
                        1;
                    quadScreenValueAddress = &scratchpad->screenCoordinates[2];
                    gte_stsxy0_precise(subdividedQuadGteScreen1);
                    gte_stsxy1_precise_keep_address(quadScreenValueAddress);
                    gte_stsxy2_precise_keep_address(subdividedQuadGteScreen3);
                    {
                        FxCommonFt4Packet *quadPacket0 =
                            ((FxCommonFt4Packet *)(quadUv3Cursor - 18));
                        quadPacket0->tag.bytes.length = quadLength;
                        quadPacket0->color = var_fp->color;
                        quadPacket0->xy0 = scratchpad->screenCoordinates[0];
                        quadPacket0->xy1 = scratchpad->midpointScreenCoordinates[0];
                        quadPacket0->xy2 = scratchpad->midpointScreenCoordinates[1];
                        quadPacket0->xy3 = scratchpad->midpointScreenCoordinates[2];
                        quadPacket0->tpage =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))->textures[0].tpage;
                        quadPacket0->clut =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))->textures[0].clut;
                        quadPacket0->uv0 = (u16)quadUv0->packed;
                        quadPacket0->uv1 = (u16)scratchpad->midpointUv[0].packed;
                        quadPacket0->uv2 = (u16)scratchpad->midpointUv[1].packed;
                        do {
                            quadPacket0->uv3 = (u16)scratchpad->midpointUv[2].packed;
                        } while (0);
                        quadUv3Cursor += 20;
                        {
                            u32 earlyQuadTag0 = var_t1_2->tag.packed;
                            orderBuffer = D_8019C9C0;
                            earlyQuadTag0 &= quadLengthMask;
                            earlyQuadTag0 |=
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                 quadAddressMask);
                            var_t1_2->tag.packed = earlyQuadTag0;
                        }
                        {
                            u32 firstQuadOtWord;
                            u32 firstQuadOtAddress;
                            firstQuadOtWord =
                                orderBuffer->allocation[scratchpad->orderingDepth].packed;
                            firstQuadOtWord &= quadLengthMask;
                            firstQuadOtAddress = ((u32)var_t1_2) & quadAddressMask;
                            asm volatile("" : "=r"(firstQuadOtAddress) : "0"(firstQuadOtAddress));
                            orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                firstQuadOtWord | firstQuadOtAddress;
                        }
                    }
                    midpointScreen2Address = &scratchpad->midpointScreenCoordinates[2];
                    {
                        FxCommonFt4Packet *quadPacket1 =
                            ((FxCommonFt4Packet *)(quadUv3Cursor - 18));
                        quadPacket1->tag.bytes.length = quadLength;
                        quadPacket1->color = var_fp->color;
                        quadPacket1->xy0 = scratchpad->midpointScreenCoordinates[0];
                        quadPacket1->xy1 = scratchpad->screenCoordinates[1];
                        quadPacket1->xy2 = *midpointScreen2Address;
                        quadPacket1->xy3 = scratchpad->midpointScreenCoordinates[3];
                        quadPacket1->tpage = (u16)((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                                 ->textures[0]
                                                 .tpage;
                        quadPacket1->clut = (u16)((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                                ->textures[0]
                                                .clut;
                        quadPacket1->uv0 = (u16)scratchpad->midpointUv[0].packed;
                        quadPacket1->uv1 = (u16)quadUv1->packed;
                        quadPacket1->uv2 = (u16)scratchpad->midpointUv[2].packed;
                        asm volatile("" : "=r"(quadUv3Cursor) : "0"(quadUv3Cursor));
                        *quadUv3Cursor = scratchpad->midpointUv[3].packed;
                        quadUv3Cursor += 20;
                        ++var_t1_2;
                        {
                            u32 earlyQuadTag1 = var_t1_2->tag.packed;
                            orderBuffer = D_8019C9C0;
                            orderingTableBase = (u8 *)orderBuffer->allocation;
                            earlyQuadTag1 &= quadLengthMask;
                            earlyQuadTag1 |=
                                ((((u32 *)orderingTableBase)[scratchpad->orderingDepth]) &
                                 quadAddressMask);
                            var_t1_2->tag.packed = earlyQuadTag1;
                        }
                        orderBuffer->allocation[scratchpad->orderingDepth].packed =
                            (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                             quadLengthMask) |
                            (((u32)var_t1_2) & quadAddressMask);
                    }
                    {
                        u32 quadPacketLengthBits;
                        FxCommonFt4Packet *quadPacket2 =
                            ((FxCommonFt4Packet *)(quadUv3Cursor - 18));
                        quadPacket2->tag.bytes.length = quadLength;
                        quadPacket2->color = var_fp->color;
                        quadPacket2->xy0 = scratchpad->midpointScreenCoordinates[1];
                        quadPacket2->xy1 = scratchpad->midpointScreenCoordinates[2];
                        quadPacket2->xy2 = *quadScreenValueAddress;
                        quadPacket2->xy3 = scratchpad->midpointScreenCoordinates[4];
                        quadPacket2->tpage = (u16)((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                                 ->textures[0]
                                                 .tpage;
                        quadPacket2->clut =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))->textures[0].clut;
                        quadPacket2->uv0 = (u16)scratchpad->midpointUv[1].packed;
                        quadPacket2->uv1 = (u16)scratchpad->midpointUv[2].packed;
                        quadPacket2->uv2 = (u16)quadUv2->packed;

                        ++var_t1_2;
                        quadPacket2->uv3 = scratchpad->midpointUv[4].packed;
                        quadUv3Cursor += 20;
                        quadPacketLengthBits = var_t1_2->tag.packed;
                        quadPacketLengthBits &= quadLengthMask;

                        orderBuffer = D_8019C9C0;
                        var_t1_2->tag.packed =
                            (quadPacketLengthBits |
                                  ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                   quadAddressMask));
                        {
                            u32 thirdOtIndex;
                            u32 *thirdOtBase;
                            u32 *thirdOtSlot;
                            u32 thirdOtWord;
                            u32 thirdOtAddress;
                            thirdOtIndex = scratchpad->orderingDepth;

                            thirdOtBase = (u32 *)orderBuffer->allocation;
                            thirdOtSlot = (u32 *)((thirdOtIndex << 2) + (u32)thirdOtBase);
                            thirdOtWord = *thirdOtSlot & quadLengthMask;
                            thirdOtAddress = ((u32)var_t1_2) & quadAddressMask;
                            *thirdOtSlot = thirdOtWord | thirdOtAddress;
                        }
                    }
                    {
                        FxCommonFt4Packet *quadPacket3 =
                            ((FxCommonFt4Packet *)(quadUv3Cursor - 18));
                        quadPacket3->tag.bytes.length = quadLength;
                        quadPacket3->color = var_fp->color;
                        quadPacket3->xy0 = scratchpad->midpointScreenCoordinates[2];
                        quadPacket3->xy1 = *new_var;
                        quadPacket3->xy2 = scratchpad->midpointScreenCoordinates[4];
                        quadPacket3->xy3 = scratchpad->screenCoordinates[3];
                        quadPacket3->tpage =
                            ((FxCommonTexturedQuad *)(quadTextureCursor - 6))->textures[0].tpage;
                        quadPacket3->clut = (u16)((FxCommonTexturedQuad *)(quadTextureCursor - 6))
                                                ->textures[0]
                                                .clut;
                        quadPacket3->uv0 = (u16)scratchpad->midpointUv[2].packed;
                        quadPacket3->uv1 = (u16)scratchpad->midpointUv[3].packed;
                        quadPacket3->uv2 = (u16)scratchpad->midpointUv[4].packed;
                        ++var_t1_2;
                        quadPacket3->uv3 = (u16)quadUv3->packed;
                    }
                }
                quadUv3Cursor += 20;
                {
                    u32 lastQuadTag;
                    u32 lastQuadNext;
                    u32 lastQuadAddress;
                    lastQuadTag = var_t1_2->tag.packed;
                    orderBuffer = D_8019C9C0;
                    lastQuadTag &= quadLengthMask;
                    lastQuadNext =
                        ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                              quadAddressMask);
                    lastQuadTag |= lastQuadNext;
                    var_t1_2->tag.packed = lastQuadTag;
                    lastQuadAddress = ((u32)var_t1_2) & quadAddressMask;
                    ++var_t1_2;
                    orderBuffer->allocation[scratchpad->orderingDepth].packed =
                        (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                         quadLengthMask) |
                        lastQuadAddress;
                }
            nextTexturedQuad:
                var_s4 += 1;
                quadTextureCursor += 72;
                quadVertex3 += 9;
                var_s5 += 0x48;
                var_s3 += 0x48;
                var_s2 += 0x48;
                ++var_fp;
            } while (var_s4 < var_t8->counts[5]);
        }
        var_s3 = (u8 *)var_t1_2;
        {
            s32 primitiveIndex6;
            primitiveIndex6 = 0;
            primitiveCount = var_t8->counts[6];
            if (primitiveCount > 0) {
                s32 orderingDepth6;
                register FxCommonColoredTexturedTriangle *transientCursor asm("$25");
                u32 gt3Tag;
                s32 temp_v0_2;
                u32 addressMask6 = 0xFFFFFF;
                u32 lengthMask6 = 0xFF000000;
                {
                    var_s1 = var_s3 + 0x1C;
                    transientCursor = gt3Slot.cursor;
                    var_s2 = (u8 *)&transientCursor->colors[2];
                }
                do {
                    transientCursor = gt3Slot.cursor;
                    temp_v0_2 = func_80079384(
                        &transientCursor->vertices[0], &transientCursor->vertices[1],
                        &transientCursor->vertices[2], &scratchpad->screenCoordinates[0],
                        &scratchpad->screenCoordinates[1], &scratchpad->screenCoordinates[2],
                        &scratchpad->perspective, &scratchpad->orderingDepth,
                        &scratchpad->transformFlags);
                    orderingDepth6 = (scratchpad->orderingDepth + D_801EA5E0) >> 2;
                    scratchpad->orderingDepth = orderingDepth6;
                    if ((temp_v0_2 > 0) && (((orderingDepth6 > 0) && (orderingDepth6 < 0x1000)))) {
                        u32 gt3PreviousLink;
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->tag.bytes.length = 9;
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->xy0 =
                            scratchpad->screenCoordinates[0];
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->xy1 =
                            scratchpad->screenCoordinates[1];
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->xy2 =
                            scratchpad->screenCoordinates[2];
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->texture0 = *(
                            (u32 *)&((FxCommonColoredTexturedTriangle *)(var_s2 - 8))->texture.uv0);
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->texture1 = *(
                            (u32 *)&((FxCommonColoredTexturedTriangle *)(var_s2 - 8))->texture.uv1);
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->texture2 = *(
                            (u32 *)&((FxCommonColoredTexturedTriangle *)(var_s2 - 8))->texture.uv2);
                        transientCursor = gt3Slot.cursor;
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->color0 =
                            *((u32 *)&transientCursor->colors[0]);
                        ((FxCommonGt3Packet *)(var_s1 - 0x1c))->color1 =
                            *((u32 *)&((FxCommonColoredTexturedTriangle *)(var_s2 - 8))->colors[1]);
                        *((u32 *)&((FxCommonGt3Packet *)(var_s1 - 0x1c))->color2) =
                            *((u32 *)&((FxCommonColoredTexturedTriangle *)(var_s2 - 8))->colors[2]);
                        var_s1 += 0x28;
                        gt3Tag = ((FxCommonPacketTag *)var_s3)->packed;
                        orderBuffer = D_8019C9C0;
                        gt3Tag &= lengthMask6;
                        gt3PreviousLink =
                            ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                  addressMask6);
                        gt3Tag |= gt3PreviousLink;
                        ((FxCommonPacketTag *)var_s3)->packed = gt3Tag;
                        {

                            u32 packetAddress6 = ((u32)var_s3) & addressMask6;
                            var_s3 += 0x28;
                            orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                 lengthMask6) |
                                packetAddress6;
                        }
                    }
                    primitiveIndex6 += 1;
                    var_s2 += 0x3C;
                    primitiveCount = var_t8->counts[6];
                    transientCursor = gt3Slot.cursor;
                    ++transientCursor;
                    gt3Slot.cursor = transientCursor;
                } while (primitiveIndex6 < primitiveCount);
            }
        }
        {
            s32 primitiveIndex7;
            primitiveIndex7 = 0;
            primitiveCount = var_t8->counts[7];
            if (primitiveCount > 0) {
                u32 gt4LengthMask = 0xFF000000u;
                register FxCommonColoredTexturedQuad *transientCursor asm("$25");
                u32 gt4Tag;
                s32 temp_v0_3;
                s32 temp_v1_14;
                u32 addressMask7 = 0xFFFFFF;
                if (var_s2) {
                    var_s1 = var_s3 + 0x30;
                    transientCursor = gt4Slot.cursor;
                    var_s2 = (u8 *)&transientCursor->texture.uv3;
                } else {
                    var_s1 = var_s3 + 0x30;
                    transientCursor = gt4Slot.cursor;
                    var_s2 = (u8 *)&transientCursor->texture.uv3;
                }
                do {
                    transientCursor = gt4Slot.cursor;
                    temp_v0_3 = func_80079414(
                        &transientCursor->vertices[0], &transientCursor->vertices[1],
                        &transientCursor->vertices[2], &transientCursor->vertices[3],
                        &scratchpad->screenCoordinates[0], &scratchpad->screenCoordinates[1],
                        &scratchpad->screenCoordinates[2], &scratchpad->screenCoordinates[3],
                        &scratchpad->perspective, &scratchpad->orderingDepth,
                        &scratchpad->transformFlags);
                    temp_v1_14 = (scratchpad->orderingDepth + D_801EA5E0) >> 2;
                    scratchpad->orderingDepth = temp_v1_14;
                    if ((temp_v0_3 > 0) && (((temp_v1_14 > 0) && (temp_v1_14 < 0x1000)))) {
                        u32 gt4PreviousLink;
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->tag.bytes.length = 0xC;
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->xy0 =
                            scratchpad->screenCoordinates[0];
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->xy1 =
                            scratchpad->screenCoordinates[1];
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->xy2 =
                            scratchpad->screenCoordinates[2];
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->xy3 =
                            scratchpad->screenCoordinates[3];
                        transientCursor = gt4Slot.cursor;
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->color0 =
                            *((u32 *)&transientCursor->colors[0]);
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->color1 =
                            *((u32 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->colors[1]);
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->color2 =
                            *((u32 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->colors[2]);
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->color3 =
                            *((u32 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->colors[3]);
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->texture0 =
                            *((u32 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->texture.uv0);
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->texture1 =
                            *((u32 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->texture.uv1);
                        ((FxCommonGt4Packet *)(var_s1 - 0x30))->texture2 =
                            *((u32 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->texture.uv2);
                        *((u16 *)&((FxCommonGt4Packet *)(var_s1 - 0x30))->uv3) =
                            *((u16 *)&((FxCommonColoredTexturedQuad *)(var_s2 - 26))->texture.uv3);
                        var_s1 += 0x34;
                        gt4Tag = ((FxCommonPacketTag *)var_s3)->packed;
                        orderBuffer = D_8019C9C0;
                        gt4Tag &= gt4LengthMask;
                        gt4PreviousLink =
                            ((orderBuffer->allocation[scratchpad->orderingDepth].packed) &
                                  addressMask7);
                        gt4Tag |= gt4PreviousLink;
                        ((FxCommonPacketTag *)var_s3)->packed = gt4Tag;
                        {

                            u32 packetAddress7 = ((u32)var_s3) & addressMask7;
                            var_s3 += 0x34;
                            orderBuffer->allocation[scratchpad->orderingDepth].packed =
                                (orderBuffer->allocation[scratchpad->orderingDepth].packed &
                                 gt4LengthMask) |
                                packetAddress7;
                        }
                    }
                    primitiveIndex7 += 1;
                    var_s2 += 0x4C;
                    primitiveCount = var_t8->counts[7];
                    transientCursor = gt4Slot.cursor;
                    ++transientCursor;
                    gt4Slot.cursor = transientCursor;
                } while (primitiveIndex7 < primitiveCount);
            }
        }
        D_8019C9C0->data = var_s3;
    }
}
